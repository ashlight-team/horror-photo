#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "FrustumVisibilityComponent.generated.h"

USTRUCT(BlueprintType)
struct FVisibleObjectInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) TObjectPtr<AActor>        ObjectRef = nullptr;
    UPROPERTY(BlueprintReadOnly) TSubclassOf<AActor>        ObjectClass = nullptr;
    UPROPERTY(BlueprintReadOnly) float                      Distance = 0.f;
    UPROPERTY(BlueprintReadOnly) FVector                    LocalPosition = FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly) FVector                    WorldPosition = FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly) int32                      TotalPixels = 0;
    UPROPERTY(BlueprintReadOnly) int32                      VisiblePixels = 0;
    UPROPERTY(BlueprintReadOnly) float                      VisibilityRatio = 0.f; // 0..1
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisibilityProcessed, const TArray<FVisibleObjectInfo>&, Results);

/**
 * Drop-in component that reports per-actor visibility (pixel-accurate) from a SceneCapture2D.
 * Heavy counting runs on a background thread; GT only does capture + readback.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTHORRORPHOTO_API UFrustumVisibilityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFrustumVisibilityComponent();

    /** Auto-create and configure a SceneCapture + RT at BeginPlay if true. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility|Setup")
    bool bAutoCreateCapture = true;

    /** Render target resolution (lower = faster). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility|Setup", meta=(ClampMin="64", ClampMax="4096"))
    int32 RenderTargetWidth = 1280;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility|Setup", meta=(ClampMin="64", ClampMax="4096"))
    int32 RenderTargetHeight = 720;

    /** Post-process material that writes CustomStencil to color (see setup notes). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility|Setup")
    TObjectPtr<UMaterialInterface> StencilToColorPP;

    /** Optional external capture; if null and AutoCreate enabled, the component will create one. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility|Runtime")
    TObjectPtr<USceneCaptureComponent2D> Capture = nullptr;

    /** Results arrive here on the Game Thread. */
    UPROPERTY(BlueprintAssignable, Category="Visibility|Runtime")
    FOnVisibilityProcessed OnVisibilityProcessed;

    /** Fully configure/refresh the capture (RT, flags, PP, etc.). Safe to call at runtime. */
    UFUNCTION(BlueprintCallable, Category="Visibility|Setup")
    void InitializeCapture();

    /**
     * Main entry point: run async visibility query for up to 254 actors.
     * Results are emitted via OnVisibilityProcessed.
     */
    UFUNCTION(BlueprintCallable, Category="Visibility|Runtime")
    void GetVisibleObjectsAsync(const TArray<AActor*>& Candidates);

    /** Convenience: assign stable IDs to any new actors. */
    UFUNCTION(BlueprintCallable, Category="Visibility|Runtime")
    void RegisterActors(const TArray<AActor*>& Actors);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY() TObjectPtr<UTextureRenderTarget2D> RT = nullptr;

    TMap<TWeakObjectPtr<AActor>, uint8> ActorToID;
    TMap<uint8, TWeakObjectPtr<AActor>> IDToActor;

    TMap<TWeakObjectPtr<AActor>, int32> CachedTotalPixels;

    void ConfigureCapture();
    void EnsureRenderTarget();
    void AssignIDForActor(AActor* Actor);

    // Isolated full-mask count for a single actor (slow path; cached).
    int32 ComputeFullSilhouettePixels_Isolated(AActor* Actor);

    bool ReadRenderTargetPixels(TArray<FColor>& OutPixels) const;

    struct FBakedMeta
    {
        // We avoid raw UObject usage on BG thread; indices map back after.
        int32   Index = -1;
        uint8   ID = 0;
        FVector World = FVector::ZeroVector;
        FVector Local = FVector::ZeroVector;
        float   Distance = 0.f;
        int32   TotalPixels = 0;
    };

public:
	/** Iterate per object: current=255, others=254; counts R==255. Emits OnVisibilityProcessed at the end. */
	UFUNCTION(BlueprintCallable, Category="Visibility|Runtime")
	void GetVisibleObjectsIterativeAsync(const TArray<AActor*>& Candidates);

private:
	void SetStencilForIterPass(AActor* Current, const TArray<AActor*>& All);
	int32 CountChannelR(const TArray<FColor>& Pixels, uint8 Expected) const;

};
