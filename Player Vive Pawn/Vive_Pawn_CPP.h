

#pragma once

#include "GameFramework/Pawn.h"
#include "LevelSelectMonitor.h"
#include "Vive_Pawn_CPP.generated.h"

UCLASS()
class LIGHTREPAIRTEAM4_API AVive_Pawn_CPP : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVive_Pawn_CPP();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Components
	// Root of the Player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class USceneComponent* ActorRoot;

	// Headset Mesh -- This should be invisible to the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class UStaticMeshComponent* HMDMesh;

	// Left Hand
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class UMotionControllerComponent* MCCLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class UStaticMeshComponent* LeftMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class UStaticMeshComponent* LeftOverlap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class UStaticMeshComponent* LeftTrackpadMarker;

	float LeftTrackpadX = 0;
	float LeftTrackpadY = 0;

	UFUNCTION()
		void LeftTrackpadX_Update(float in);

	UFUNCTION()
		void LeftTrackpadY_Update(float in);

	UFUNCTION()
		void UpdateTouchpoint_Left();

	UFUNCTION()
		void UpdateMCPosition_Left();

	UFUNCTION()
		void LeftTrigger_Pressed();

	UFUNCTION()
		void LeftTrigger_Released();

	UFUNCTION()
		void LeftTouchpad_Pressed();

	UFUNCTION()
		void LeftMenu_Pressed();

	UFUNCTION()
		void LeftMenu_Released();

	UFUNCTION()
		void OnBeginOverlap_Left(class UPrimitiveComponent* Comp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnEndOverlap_Left(class UPrimitiveComponent* Comp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	TArray<AActor*> SOI_Left;
	class AActor* ActorHeld_Left;

	// Right Hand
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class UMotionControllerComponent* MCCRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class UStaticMeshComponent* RightMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class UStaticMeshComponent* RightOverlap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		class UStaticMeshComponent* RightTrackpadMarker;


	float RightTrackpadX = 0;
	float RightTrackpadY = 0;

	UFUNCTION()
		void RightTrackpadX_Update(float in);

	UFUNCTION()
		void RightTrackpadY_Update(float in);

	UFUNCTION()
		void UpdateTouchpoint_Right();

	UFUNCTION()
		void UpdateMCPosition_Right();

	UFUNCTION()
		void RightTrigger_Pressed();

	UFUNCTION()
		void RightTrigger_Released();

	UFUNCTION()
		void RightTouchpad_Pressed();

	UFUNCTION()
		void RightMenu_Pressed();

	UFUNCTION()
		void RightMenu_Released();

	UFUNCTION()
		void OnBeginOverlap_Right(class UPrimitiveComponent* Comp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnEndOverlap_Right(class UPrimitiveComponent* Comp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	TArray<AActor*> SOI_Right;
	class AActor* ActorHeld_Right;

	UFUNCTION(BlueprintCallable, Category = Components)
		void SetPickUp(bool PickUp);

	UFUNCTION(BlueprintCallable, Category = Components)
		void DropAllHeld();

	UFUNCTION(BlueprintNativeEvent)
		void GoToLevelSelect();

	UFUNCTION(BlueprintNativeEvent)
		void MenuDoubleTapExit();

	UFUNCTION(BlueprintNativeEvent)
		void GoToLevel();

	UFUNCTION(BlueprintNativeEvent)
		void PlayHapticFeedback(float Intensity, float Duration, bool bLeft, bool bRight);

	UFUNCTION()
		void MenuTouchpad(bool bLeft, bool bRight);

	UPROPERTY()
		ALevelSelectMonitor* LevelMonitor;

	UFUNCTION(BlueprintCallable, Category = "Level Select")
		void SetMonitor(ALevelSelectMonitor* NewMonitor = nullptr);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Select")
		float LongTimeThreshold = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Select")
		float ShortTimeThreshold = 0.50f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Select")
		float TrackpadThreshold = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Select")
		float MenuHoldReturn = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Select")
		float MenuHoldQuit = 2.0f;

	//////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Haptic Feedback")
		float SmallIntensity = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Haptic Feedback")
		float LargeIntensity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Haptic Feedback")
		float SmallDuration = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Haptic Feedback")
		float LargeDuration = 0.1f;

	//////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
		bool bTestLevelSelect = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bInLevelSelect = false;

private:
	bool bCanPickUp = true;
	float TimeSinceLastMove = 0;
	int LastDir = 0;
	float TimeSinceLastMenuPress = 0;

	float MenuHeld_Left = -1.0f;
	float MenuHeld_Right = -1.0f;
};
