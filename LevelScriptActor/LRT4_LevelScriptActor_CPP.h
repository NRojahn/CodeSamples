

#pragma once

#include "Engine/LevelScriptActor.h"
#include "Receiver_CPP.h"
#include "LRT4_LevelScriptActor_CPP.generated.h"

/**
 * 
 */
USTRUCT()
 struct FLightSetStruct
 {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* LightMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AStaticMeshActor*> LightsInSet;
	
 };

UCLASS()
class LIGHTREPAIRTEAM4_API ALRT4_LevelScriptActor_CPP : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	// Material to replace light's original material when they come on
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timing")
	int32 AtLightingSet = 0;

	// Arrays of all lights to turn on at the end of the level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	TArray<FLightSetStruct> LightSets;

	/*
	Current State of the Level
		0 - Fade-In
		1 - Gameplay
		2 - Level Completed
		3 - Light Sets
		4 - Fade-Out
		5 - Load Next Level
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timing")
	uint8 LevelState = 0;

	// Time between states, in seconds.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float SequenceTime = 1.0f;

	// Time left in current state.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timing")
	float StateTimer = 0.0f;

	// Timer for testing player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timing")
	float PlayerTime = 0.0f;

	// Function that is called when the next level is to be streamed.
	UFUNCTION(BlueprintImplementableEvent, Category = "Required")
		void LoadNextLevel();

	// Function that is called when the next level is to be streamed.
	UFUNCTION(BlueprintImplementableEvent, Category = "Required")
		void FadeIn();

	// Function that is called when the next level is to be streamed.
	UFUNCTION(BlueprintImplementableEvent, Category = "Required")
		void FadeOut();

	// Array of all Recievers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Data")
	TArray<AReceiver_CPP*> Recievers;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Required")
	void SaveTime(FName LevelName);





private:
	void StateFadeIn(float DeltaTime);
	void StateGameplay(float DeltaTime);
	void StateCompleted(float DeltaTime);
	void StateLights(float DeltaTime);
	void StateFadeOut(float DeltaTime);
	void StateNextLevel(float DeltaTime);
};
