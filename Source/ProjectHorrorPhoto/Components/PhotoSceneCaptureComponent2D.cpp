#include "PhotoSceneCaptureComponent2D.h"

#include "EngineUtils.h"
#include "ImageUtils.h"
#include "RenderingThread.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Runtime/RenderCore/Public/RenderingThread.h"


FPhotoData UPhotoSceneCaptureComponent2D::TakePhoto(TArray<AActor*> IgnoreActors)
{
	FPhotoData Result;

	if (CaptureToPNG(Result.ImageData))
	{
		FEngineShowFlags OldShowFlags = ShowFlags;
		TEnumAsByte<enum ESceneCaptureSource> OldCaptureSource = CaptureSource;
		ESceneCapturePrimitiveRenderMode OldPrimMode = PrimitiveRenderMode;
		TObjectPtr<class UTextureRenderTarget2D> OldRT = TextureTarget;
		
		
		Result.VisibleActors = CaptureVisibleActors(0.1f, IgnoreActors);
		
		CaptureSource = OldCaptureSource;
		PrimitiveRenderMode = OldPrimMode;
		ShowFlags = OldShowFlags;
		TextureTarget = OldRT;

		ClearShowOnlyComponents();
		ClearHiddenComponents();

		bCaptureOnMovement = true;
		bCaptureEveryFrame = true;
		
	}

	return Result;
}

bool UPhotoSceneCaptureComponent2D::CaptureToPNG(TArray<uint8>& OutImage)
{
	if (!TextureTarget) return false;

	FTextureRenderTargetResource* RTResource = TextureTarget->GameThread_GetRenderTargetResource();
	TArray<FColor> Bitmap;
	if (!RTResource->ReadPixels(Bitmap)) return false;

	FImageUtils::CompressImageArray(TextureTarget->SizeX, TextureTarget->SizeY, Bitmap, OutImage);
	return true;
}


TArray<FActorCaptureInfo> UPhotoSceneCaptureComponent2D::CaptureVisibleActors(float MinCoveragePercent, TArray<AActor*> IgnoreActors)
{
	TArray<FActorCaptureInfo> Result;

	if (!GetWorld()) return Result;

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor->IsPendingKillPending() || Actor->IsHidden()) continue;
		if (Actor->GetRootComponent() && !Actor->GetRootComponent()->IsVisible()) continue;
		if (!IsInFOV(Actor)) continue;

		float CapturedPixels = 0.f;
		float TotalPixels = 0.f;
		CountRenderedCoverage(Actor, CapturedPixels, TotalPixels);
		if (TotalPixels <= 0.f) continue;
		float Coverage = CapturedPixels / TotalPixels;
		if (Coverage < MinCoveragePercent) continue;
		if (IgnoreActors.Contains(Actor)) continue;
		
		
		FActorCaptureInfo Info;
		Info.Actor = Actor;
		Info.CoveragePercent = Coverage * 100.f;
		Info.CapturedPixels = CapturedPixels;
		Info.TotalPixels = TotalPixels;
		Info.DistanceFromCamera = FVector::Dist(GetComponentLocation(), Actor->GetActorLocation());
		Info.RelativeLocation = GetComponentTransform().InverseTransformPosition(Actor->GetActorLocation());
		Result.Add(Info);
	}

	return Result;
}

bool UPhotoSceneCaptureComponent2D::IsInFOV(const AActor* Actor)
{
	if (!Actor) return false;

	FVector ActorLocation = Actor->GetActorLocation();
	FRotator ActorRotation = Actor->GetActorRotation();

	FVector ComponentLocation = GetComponentLocation();

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ComponentLocation, ActorLocation);
	float YawDiff = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(LookAtRotation, GetComponentRotation()).Yaw);

	return YawDiff <= (FOVAngle / 2.f);
}

static void ReadDepthToFloat16(UTextureRenderTarget2D* RT, TArray<FFloat16Color>& OutPixels)
{
	check(IsInGameThread());
	OutPixels.Reset();
	if (!RT) return;

	FTextureRenderTargetResource* Res = RT->GameThread_GetRenderTargetResource();
	if (!Res) return;

	// This reads RGBA16f; for Scene/DeviceDepth capture, depth is in R
	Res->ReadFloat16Pixels(OutPixels);
}

static bool ReadDepthToFloat16Blocking(UTextureRenderTarget2D* RT, TArray<FFloat16Color>& OutPixels)
{
	if (!RT) return false;
	FTextureRenderTargetResource* Res = RT->GameThread_GetRenderTargetResource();
	if (!Res) return false;
	OutPixels.Reset();
	Res->ReadFloat16Pixels(OutPixels);
	return OutPixels.Num() == (RT->SizeX * RT->SizeY);
}


void UPhotoSceneCaptureComponent2D::CountRenderedCoverage(AActor* Actor, float& OutCaptured, float& OutTotal)
{
	OutCaptured = 0.f;
	OutTotal    = 0.f;

	if (!Actor || !TextureTarget || TextureTarget->SizeX <= 0 || TextureTarget->SizeY <= 0) return;

	const int32 W = TextureTarget->SizeX, H = TextureTarget->SizeY;

	bCaptureEveryFrame = false;
	bCaptureOnMovement = false;

	UTextureRenderTarget2D* RT_With    = UKismetRenderingLibrary::CreateRenderTarget2D(this, W, H, RTF_RGBA16f);
	UTextureRenderTarget2D* RT_Without = UKismetRenderingLibrary::CreateRenderTarget2D(this, W, H, RTF_RGBA16f);
	UTextureRenderTarget2D* RT_Sil     = UKismetRenderingLibrary::CreateRenderTarget2D(this, W, H, RTF_RGBA16f);
	if (!RT_With || !RT_Without || !RT_Sil) return;

	auto ReadDepth = [](UTextureRenderTarget2D* RT, TArray<FFloat16Color>& Out) -> bool
	{
		if (!RT) return false;
		if (auto* Res = RT->GameThread_GetRenderTargetResource())
		{
			Out.Reset();
			Res->ReadFloat16Pixels(Out); // blocks until RT is ready
			return Out.Num() == (RT->SizeX * RT->SizeY);
		}
		return false;
	};

	PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
	ClearHiddenComponents();
	ClearShowOnlyComponents();
	CaptureSource = ESceneCaptureSource::SCS_DeviceDepth;

	TextureTarget = RT_With;
	CaptureScene();
	TArray<FFloat16Color> With;
	if (!ReadDepth(RT_With, With)) return;

	HideActorComponents(Actor, /*bIncludeFromChildActors=*/true);

	TextureTarget = RT_Without;
	CaptureScene();
	TArray<FFloat16Color> Without;
	if (!ReadDepth(RT_Without, Without)) return;

	PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	ClearHiddenComponents();
	ShowOnlyActorComponents(Actor, /*bIncludeFromChildActors=*/true);

	TextureTarget = RT_Sil;
	CaptureSource = ESceneCaptureSource::SCS_DeviceDepth;
	CaptureScene();
	TArray<FFloat16Color> Sil;
	if (!ReadDepth(RT_Sil, Sil)) return;

	const int32 Num = With.Num();
	if (Without.Num() != Num || Sil.Num() != Num) return;

	const float EPS = 1e-5f;
	const float BG  = 1.0f; // background in device depth

	float Visible = 0.f;
	float Silhouette = 0.f;

	for (int32 i = 0; i < Num; ++i)
	{
		const float Dwith = With[i].R;     // full scene (actor visible if nearest differs)
		const float Dwo   = Without[i].R;  // scene without actor
		const float Da    = Sil[i].R;      // actor-only depth (silhouette)

		const bool bActorCovers = (Da < BG - EPS); // independent of reversed-Z
		
		const bool bIsVisible = FMath::Abs(Dwith - Dwo) > EPS;

		if (bActorCovers)            Silhouette += 1.f;
		if (bActorCovers && bIsVisible) Visible   += 1.f;
	}

	if (Visible <= 0.f)
	{
		OutCaptured = 0.f;
		OutTotal    = 0.f;
	}
	else
	{
		OutCaptured = Visible;
		OutTotal    = Silhouette; // use actor area for % if you need it
	}
}

