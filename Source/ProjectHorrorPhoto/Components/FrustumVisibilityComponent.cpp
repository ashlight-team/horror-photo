#include "FrustumVisibilityComponent.h"
#include "Async/Async.h"
#include "Components/MeshComponent.h"
#include "Engine/Texture.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

UFrustumVisibilityComponent::UFrustumVisibilityComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UFrustumVisibilityComponent::BeginPlay()
{
    Super::BeginPlay();

    if (bAutoCreateCapture)
    {
        if (!Capture)
        {
            AActor* Owner = GetOwner();
            if (Owner)
            {
                Capture = NewObject<USceneCaptureComponent2D>(Owner, TEXT("FrustumVis_SceneCapture"));
                Capture->SetupAttachment(Owner->GetRootComponent());
                Capture->RegisterComponent();
                Capture->SetRelativeLocation(FVector::ZeroVector);
                Capture->SetRelativeRotation(FRotator::ZeroRotator);
            }
        }
        InitializeCapture();
    }
}

void UFrustumVisibilityComponent::InitializeCapture()
{
    if (!Capture)
    {
        UE_LOG(LogTemp, Warning, TEXT("InitializeCapture: Capture is null. Set bAutoCreateCapture or assign a Capture."));
        return;
    }

    EnsureRenderTarget();
    ConfigureCapture();

    if (!StencilToColorPP)
    {
        UE_LOG(LogTemp, Warning, TEXT("InitializeCapture: StencilToColorPP is not set. See setup notes for required PP material."));
    }
}

void UFrustumVisibilityComponent::EnsureRenderTarget()
{
    if (!RT)
    {
        RT = NewObject<UTextureRenderTarget2D>(this, TEXT("FrustumVis_RT"));
        RT->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8; // PF_B8G8R8A8
        RT->bAutoGenerateMips = false;
        RT->ClearColor = FLinearColor::Black;
        RT->TargetGamma = 1.0f; // linear
        RT->SRGB = false;
        RT->AddressX = TA_Clamp;
        RT->AddressY = TA_Clamp;
        RT->Filter = TF_Bilinear;
    }

    RT->InitCustomFormat(RenderTargetWidth, RenderTargetHeight, PF_B8G8R8A8, /*ForceLinearGamma=*/true);
    RT->UpdateResourceImmediate(true);

    Capture->TextureTarget = RT;
}

void UFrustumVisibilityComponent::ConfigureCapture()
{
    // No “every frame” captures; we call CaptureScene() on demand.
    Capture->bCaptureEveryFrame = false;
    Capture->bCaptureOnMovement = false;

    Capture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

    FEngineShowFlags& SF = Capture->ShowFlags;
    SF.SetTemporalAA(false);
    SF.SetAntiAliasing(false);
    SF.SetMotionBlur(false);
    SF.SetBloom(false);
    SF.SetEyeAdaptation(false);
    SF.SetTonemapper(false);
    SF.SetAmbientOcclusion(false);

    Capture->ClearShowOnlyComponents();

    Capture->PostProcessSettings.WeightedBlendables.Array.Reset();
    if (StencilToColorPP)
    {
        UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(StencilToColorPP, this);
        Capture->PostProcessSettings.WeightedBlendables.Array.Add(FWeightedBlendable(1.f, MID));
        Capture->PostProcessBlendWeight = 1.f;
    }
}

void UFrustumVisibilityComponent::AssignIDForActor(AActor* Actor)
{
    if (!Actor || ActorToID.Contains(Actor)) return;

    static uint8 NextID = 1; // 0 = background
    if (NextID == 0) ++NextID; // safety

    const uint8 ID = NextID++;
    ActorToID.Add(Actor, ID);
    IDToActor.Add(ID, Actor);

    TArray<UMeshComponent*> Meshes;
    Actor->GetComponents<UMeshComponent>(Meshes);
    for (UMeshComponent* MC : Meshes)
    {
        if (!MC) continue;
        MC->SetRenderCustomDepth(true);
        MC->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_Default);
        MC->SetCustomDepthStencilValue(ID);
        MC->MarkRenderStateDirty();
    }
}

void UFrustumVisibilityComponent::RegisterActors(const TArray<AActor*>& Actors)
{
    for (AActor* A : Actors) { AssignIDForActor(A); }
}

bool UFrustumVisibilityComponent::ReadRenderTargetPixels(TArray<FColor>& OutPixels) const
{
    if (!RT) return false;

    FTextureRenderTargetResource* RTRes = RT->GameThread_GetRenderTargetResource();
    if (!RTRes) return false;

    FReadSurfaceDataFlags ReadFlags(RCM_UNorm);
    ReadFlags.SetLinearToGamma(false); // we wrote linear values
    OutPixels.Reset();
    const bool bOK = RTRes->ReadPixels(OutPixels, ReadFlags);
    return bOK && OutPixels.Num() == (RenderTargetWidth * RenderTargetHeight);
}

int32 UFrustumVisibilityComponent::ComputeFullSilhouettePixels_Isolated(AActor* Actor)
{
    if (!Actor || !Capture || !RT) return 0;

    Capture->ClearShowOnlyComponents();
    Capture->ShowOnlyActorComponents(Actor);

    Capture->CaptureScene();

    TArray<FColor> Pix;
    if (!ReadRenderTargetPixels(Pix)) return 0;

    int32 Count = 0;
    for (const FColor& C : Pix)
    {
        if (C.R != 0) ++Count;
    }
    return Count;
}

void UFrustumVisibilityComponent::GetVisibleObjectsAsync(const TArray<AActor*>& InCandidates)
{
    if (!Capture || !RT || !StencilToColorPP)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetVisibleObjectsAsync: not initialized (Capture/RT/PP missing). Call InitializeCapture()."));
        OnVisibilityProcessed.Broadcast({});
        return;
    }

    // Hard cap for MVP (stencil is 8-bit; 254 usable IDs)
    if (InCandidates.Num() > 254)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetVisibleObjectsAsync: %d candidates > 254. Clamping to first 254 (MVP)."), InCandidates.Num());
    }

    TArray<AActor*> Candidates;
    Candidates.Reserve(FMath::Min(InCandidates.Num(), 254));
    int32 Taken = 0;
    for (AActor* A : InCandidates)
    {
        if (!A) continue;
        if (Taken >= 254) break;
        AssignIDForActor(A);
        Candidates.Add(A);
        ++Taken;
    }

    Capture->ClearShowOnlyComponents();
    for (AActor* A : Candidates)
    {
        Capture->ShowOnlyActorComponents(A);
    }

    const FTransform CamXf = Capture->GetComponentTransform();
    const FVector CamLoc   = CamXf.GetLocation();

    TArray<FBakedMeta> Meta;
    Meta.Reserve(Candidates.Num());

    TArray<TWeakObjectPtr<AActor>> ActorOrder;
    ActorOrder.Reserve(Candidates.Num());

    for (int32 i = 0; i < Candidates.Num(); ++i)
    {
        AActor* A = Candidates[i];
        const uint8 ID = ActorToID.FindRef(A);
        const FVector W = A->GetActorLocation();
        const FVector L = CamXf.InverseTransformPosition(W);
        const float   D = FVector::Dist(W, CamLoc);

        int32* FoundTotal = CachedTotalPixels.Find(A);
        int32 Total = FoundTotal ? *FoundTotal : 0;
        if (Total == 0)
        {
            Total = ComputeFullSilhouettePixels_Isolated(A);
            CachedTotalPixels.Add(A, Total);
        }

        FBakedMeta B;
        B.Index = i;
        B.ID = ID;
        B.World = W;
        B.Local = L;
        B.Distance = D;
        B.TotalPixels = Total;

        Meta.Add(B);
        ActorOrder.Add(A);
    }

    Capture->CaptureScene();

    TArray<FColor> Pixels;
    if (!ReadRenderTargetPixels(Pixels))
    {
        OnVisibilityProcessed.Broadcast({});
        return;
    }

    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Pixels = MoveTemp(Pixels), Meta = MoveTemp(Meta), ActorOrder = MoveTemp(ActorOrder)]() mutable
    {
        int32 VisibleByID[256]; FMemory::Memzero(VisibleByID, sizeof(VisibleByID));
        for (const FColor& C : Pixels)
        {
            const uint8 R = C.R;
            if (R != 0) { ++VisibleByID[R]; }
        }

        TArray<FVisibleObjectInfo> Out;
        Out.Reserve(Meta.Num());
        for (const FBakedMeta& B : Meta)
        {
            FVisibleObjectInfo Row;
            AActor* A = ActorOrder.IsValidIndex(B.Index) ? ActorOrder[B.Index].Get() : nullptr;

            Row.ObjectRef       = A;
            Row.ObjectClass     = A ? A->GetClass() : nullptr;
            Row.Distance        = B.Distance;
            Row.LocalPosition   = B.Local;
            Row.WorldPosition   = B.World;
            Row.TotalPixels     = B.TotalPixels;
            Row.VisiblePixels   = VisibleByID[B.ID];
            Row.VisibilityRatio = (B.TotalPixels > 0) ? (float)Row.VisiblePixels / (float)B.TotalPixels : 0.f;

            Out.Add(Row);
        }

        AsyncTask(ENamedThreads::GameThread, [this, Out = MoveTemp(Out)]() mutable
        {
            OnVisibilityProcessed.Broadcast(Out);
        });
    });
}
void UFrustumVisibilityComponent::SetStencilForIterPass(AActor* Current, const TArray<AActor*>& All)
{
    auto Apply = [&](AActor* A, uint8 ID)
    {
        if (!A) return;
        TArray<UMeshComponent*> Meshes;
        A->GetComponents<UMeshComponent>(Meshes);
        for (UMeshComponent* MC : Meshes)
        {
            if (!MC) continue;
            MC->SetRenderCustomDepth(true);
            MC->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_Default);
            MC->SetCustomDepthStencilValue(ID);
            MC->MarkRenderStateDirty();
        }
    };

    for (AActor* A : All)
    {
        Apply(A, (A == Current) ? 255 : 254);
    }
}

int32 UFrustumVisibilityComponent::CountChannelR(const TArray<FColor>& Pixels, uint8 Expected) const
{
    int32 Cnt = 0;
    for (const FColor& C : Pixels)
    {
        if (C.R == Expected) ++Cnt; // AA is off, sRGB off ⇒ exact integers
    }
    return Cnt;
}

void UFrustumVisibilityComponent::GetVisibleObjectsIterativeAsync(const TArray<AActor*>& InCandidates)
{
    if (!ensureMsgf(Capture && RT && StencilToColorPP, TEXT("Not initialized"))) { 
        OnVisibilityProcessed.Broadcast({}); 
        return; 
    }

    TArray<AActor*> Candidates;
    Candidates.Reserve(InCandidates.Num());
    for (AActor* A : InCandidates)
        if (IsValid(A)) Candidates.Add(A);

    if (Candidates.Num() == 0) { OnVisibilityProcessed.Broadcast({}); return; }

    Capture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;

    const FTransform CamXf = Capture->GetComponentTransform();
    const FVector CamLoc   = CamXf.GetLocation();

    TArray<FVisibleObjectInfo> Out; 
    Out.Reserve(Candidates.Num());

    auto CountChannelR = [](const TArray<FColor>& Pix, uint8 Expected)->int32 {
        int32 C = 0;
        for (const FColor& P : Pix) if (P.R == Expected) ++C;
        return C;
    };

    auto ReadRT = [this](TArray<FColor>& OutPixels)->bool {
        if (!RT) return false;
        FTextureRenderTargetResource* Res = RT->GameThread_GetRenderTargetResource();
        if (!Res) return false;
        FReadSurfaceDataFlags Flags(RCM_UNorm);
        Flags.SetLinearToGamma(false);
        OutPixels.Reset();
        const bool Ok = Res->ReadPixels(OutPixels, Flags);
        return Ok && OutPixels.Num() == RT->SizeX * RT->SizeY;
    };

    auto SetStencilForIterPass = [](AActor* Current, const TArray<AActor*>& All) {
        auto Apply = [](AActor* A, uint8 ID) {
            if (!A) return;
            TArray<UMeshComponent*> Meshes;
            A->GetComponents<UMeshComponent>(Meshes);
            for (UMeshComponent* MC : Meshes) {
                if (!MC || !MC->IsRegistered()) continue;
                MC->SetRenderCustomDepth(true);
                MC->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_Default);
                MC->SetCustomDepthStencilValue(ID);
                MC->MarkRenderStateDirty();
            }
        };
        for (AActor* A : All) Apply(A, (A == Current) ? 255 : 254);
    };

    for (AActor* Current : Candidates)
    {
        if (!IsValid(Current)) continue;

        const FVector W = Current->GetActorLocation();
        const FVector L = CamXf.InverseTransformPosition(W);
        const float   D = FVector::Dist(W, CamLoc);

        // --- Visible pass: Current=255, others=254; render full scene (occluders stay) ---
        SetStencilForIterPass(Current, Candidates);
        Capture->ClearShowOnlyComponents();            // include the environment
        Capture->CaptureScene();

        TArray<FColor> VisPixels;
        if (!ReadRT(VisPixels)) continue;
        const int32 VisiblePixels = CountChannelR(VisPixels, 255);

        // --- Full pass: isolate current only; Current=255 ---
        Capture->ClearShowOnlyComponents();
        Capture->ShowOnlyActorComponents(Current);
        Capture->CaptureScene();

        TArray<FColor> FullPixels;
        if (!ReadRT(FullPixels)) continue;
        const int32 TotalPixels = CountChannelR(FullPixels, 255);

        FVisibleObjectInfo Row;
        Row.ObjectRef       = Current;
        Row.ObjectClass     = Current->GetClass();
        Row.Distance        = D;
        Row.LocalPosition   = L;
        Row.WorldPosition   = W;
        Row.TotalPixels     = TotalPixels;
        Row.VisiblePixels   = VisiblePixels;
        Row.VisibilityRatio = (TotalPixels > 0) ? float(VisiblePixels) / float(TotalPixels) : 0.f;

        Out.Add(Row);
    }

    OnVisibilityProcessed.Broadcast(Out);
}

