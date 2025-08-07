// AHackerSkyEffect.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AHackerSkyEffect.generated.h"

UCLASS()
class CRYSTALSCRYPT_API AHackerSkyEffect : public AActor
{
    GENERATED_BODY()

public:
    AHackerSkyEffect();

    // ✅ [Commit] Allow ticking in editor viewport without pressing Play
    virtual bool ShouldTickIfViewportsOnly() const override { return true; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    void StartEditorTick();       // 🔁 Start ticking in editor
    void StopEditorTick();        // ⛔️ Stop ticking
    void EditorTick();            // 💡 Simulate Tick()
#endif

public:
    // ✅ Master toggle: show + animate floating words
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkyEffect", Interp)
    bool bEnableEffect = true;

    // 🔽 Controls how fast they fall
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkyEffect", Interp)
    float FallSpeed = 200.f;

    // 🔠 Custom list of words to display
    UPROPERTY(EditAnywhere, Category = "Floating Text")
    TArray<FString> FloatingWords;

    UPROPERTY(EditAnywhere, Category = "Floating Text")
    TSubclassOf<AActor> Text3DActorClass;

    UPROPERTY(EditAnywhere, Category = "Floating Text")
    UMaterialInterface* TextMaterial;

    UPROPERTY(EditAnywhere, Category = "Floating Text")
    int32 NumWordsToSpawn = 20;

    void SpawnFloatingText(const FString& Word, const FVector& Location);
    void GetLandscapeBounds(FVector& OutOrigin, FVector& OutExtent);
    FVector GetRandomSpawnLocation(const FVector& Origin, const FVector& Extent) const;
    void DestroyAllFloatingText();

private:
    UPROPERTY()
    TArray<AActor*> FloatingTextActors;

#if WITH_EDITORONLY_DATA
    FTimerHandle EditorTickHandle;

    // 🔁 Track number of simulated ticks in editor
    int32 EditorTickCount = 0;

    // 🎯 Max number of simulated ticks (e.g., 180 = 3 seconds at 60fps)
    UPROPERTY(EditAnywhere, Category = "SkyEffect", meta = (ClampMin = "1", ClampMax = "10000"))
    int32 MaxEditorTicks = 180;
#endif
};