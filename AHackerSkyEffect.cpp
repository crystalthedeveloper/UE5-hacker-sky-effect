// AHackerSkyEffect.cpp

#include "AHackerSkyEffect.h"
#include "Text3DComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EngineUtils.h"
#include "Landscape.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Editor.h"
#include "TimerManager.h"

AHackerSkyEffect::AHackerSkyEffect()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.bAllowTickOnDedicatedServer = false;
    PrimaryActorTick.bTickEvenWhenPaused = true;
}

void AHackerSkyEffect::BeginPlay()
{
    Super::BeginPlay();

    if (bEnableEffect && FloatingTextActors.Num() == 0)
    {
        FVector Origin, Extent;
        GetLandscapeBounds(Origin, Extent);

        if (FloatingWords.Num() == 0)
        {
            FloatingWords = {
                TEXT("ACCESS GRANTED"), TEXT("CRYSTAL.DEV"), TEXT("WEBFLOW"),
                TEXT("console.log"), TEXT("fetch(\"/api\")"),
                TEXT("const love = Danielle"), TEXT("export default Code;")
            };
        }

        for (int32 i = 0; i < NumWordsToSpawn; ++i)
        {
            const FString& Word = FloatingWords[FMath::RandHelper(FloatingWords.Num())];
            SpawnFloatingText(Word, GetRandomSpawnLocation(Origin, Extent));
        }
    }

#if WITH_EDITOR
    if (GIsEditor && !IsRunningGame() && bEnableEffect)
    {
        StartEditorTick();  // ✅ Start simulated ticking in editor automatically
    }
#endif
}

void AHackerSkyEffect::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bEnableEffect || FloatingTextActors.Num() == 0)
        return;

    FVector Origin, Extent;
    GetLandscapeBounds(Origin, Extent);

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    FVector CameraLocation = PC && PC->PlayerCameraManager
        ? PC->PlayerCameraManager->GetCameraLocation()
        : FVector::ZeroVector;

    for (int32 i = FloatingTextActors.Num() - 1; i >= 0; --i)
    {
        AActor* TextActor = FloatingTextActors[i];
        if (!TextActor) continue;

        FVector Pos = TextActor->GetActorLocation();

        // Face camera
        FVector LookDir = CameraLocation - Pos;
        FRotator Rotation = LookDir.Rotation();
        Rotation.Pitch = Rotation.Roll = 0.f;
        Rotation.Yaw += 180.f;
        TextActor->SetActorRotation(Rotation);

        // Fall
        Pos.Z -= FallSpeed * DeltaTime;
        TextActor->SetActorLocation(Pos);

        // Recycle if out of bounds
        if (Pos.Z < -10000.f)
        {
            TextActor->Destroy();
            FloatingTextActors.RemoveAt(i);

            const FString& Word = FloatingWords[FMath::RandHelper(FloatingWords.Num())];
            SpawnFloatingText(Word, GetRandomSpawnLocation(Origin, Extent));
        }
    }
}

void AHackerSkyEffect::SpawnFloatingText(const FString& Word, const FVector& Location)
{
    if (!Text3DActorClass) return;

    AActor* TextActor = GetWorld()->SpawnActor<AActor>(Text3DActorClass, Location, FRotator::ZeroRotator);
    if (!TextActor) return;

    if (UText3DComponent* Text = TextActor->FindComponentByClass<UText3DComponent>())
    {
        Text->SetText(FText::FromString(Word));
        Text->SetExtrude(5.f);
        Text->SetBevel(false);
        Text->SetWorldScale3D(FVector(1.f));

        if (TextMaterial)
        {
            Text->SetFrontMaterial(TextMaterial);
            Text->SetBevelMaterial(TextMaterial);
            Text->SetExtrudeMaterial(TextMaterial);
        }
    }

    FloatingTextActors.Add(TextActor);
}

void AHackerSkyEffect::DestroyAllFloatingText()
{
    for (AActor* Actor : FloatingTextActors)
    {
        if (IsValid(Actor)) Actor->Destroy();
    }
    FloatingTextActors.Empty();
}

void AHackerSkyEffect::GetLandscapeBounds(FVector& OutOrigin, FVector& OutExtent)
{
    OutOrigin = FVector::ZeroVector;
    OutExtent = FVector(50000.f);

    for (TActorIterator<ALandscape> It(GetWorld()); It; ++It)
    {
        It->GetActorBounds(false, OutOrigin, OutExtent);
        break;
    }
}

FVector AHackerSkyEffect::GetRandomSpawnLocation(const FVector& Origin, const FVector& Extent) const
{
    return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent + FVector(0, 0, 2500));
}

#if WITH_EDITOR

void AHackerSkyEffect::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AHackerSkyEffect, bEnableEffect))
    {
        if (bEnableEffect)
        {
            BeginPlay(); // Spawn new floating text
            StartEditorTick();
        }
        else
        {
            StopEditorTick();
            DestroyAllFloatingText();
        }

        GEditor->RedrawAllViewports();
    }
}

void AHackerSkyEffect::StartEditorTick()
{
    EditorTickCount = 0;

    if (!EditorTickHandle.IsValid())
    {
        GetWorld()->GetTimerManager().SetTimer(
            EditorTickHandle, this, &AHackerSkyEffect::EditorTick, 0.016f, true
        );
    }
}

void AHackerSkyEffect::StopEditorTick()
{
    if (EditorTickHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(EditorTickHandle);
    }
}

void AHackerSkyEffect::EditorTick()
{
    Tick(0.016f); // Simulate ~60fps
    EditorTickCount++;

    if (EditorTickCount >= MaxEditorTicks)
    {
        StopEditorTick();
        UE_LOG(LogTemp, Warning, TEXT("🛑 Editor ticking stopped after %d frames."), EditorTickCount);
    }
}

#endif
