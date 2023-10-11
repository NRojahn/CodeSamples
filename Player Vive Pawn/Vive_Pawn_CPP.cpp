

#include "LightRepairTeam4.h"

#include "VR_Movable_Interface.h"

/* VR Includes */
#include "HeadMountedDisplay.h"
#include "MotionControllerComponent.h"

/* OpenVR */
#include "SteamVRFunctionLibrary.h"

#include "Vive_Pawn_CPP.h"


// Sets default values
AVive_Pawn_CPP::AVive_Pawn_CPP()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ActorRoot = CreateDefaultSubobject < USceneComponent >("Object Root");
	this->RootComponent = ActorRoot;
	this->BaseEyeHeight = 0;

	// HMD - The headset
	HMDMesh = CreateDefaultSubobject < UStaticMeshComponent >("HMD Mesh");
	HMDMesh->SetCollisionProfileName("OverlapAllDynamic");
	HMDMesh->SetupAttachment(ActorRoot);
	HMDMesh->SetOwnerNoSee(true);

	// Left Hand - Motion Controller Component
	// Real World Controller, not the one used for gameplay
	MCCLeft = CreateDefaultSubobject < UMotionControllerComponent >("MCC Left");
	MCCLeft->Hand = EControllerHand::Left;
	MCCLeft->SetCollisionProfileName("OverlapAllDynamic");
	MCCLeft->SetupAttachment(ActorRoot);

	// Left Hand - Mesh
	// Virtual Controller, this is used for gameplay
	LeftMesh = CreateDefaultSubobject < UStaticMeshComponent >("Left Mesh");
	LeftMesh->SetCollisionProfileName("OverlapAllDynamic");
	LeftMesh->SetupAttachment(ActorRoot);

	// Left Hand - Trackpad marker
	// Shows where on the trackpad the user is touching, if at all.
	LeftTrackpadMarker = CreateDefaultSubobject < UStaticMeshComponent >("Left Touchpoint");
	LeftTrackpadMarker->SetCollisionProfileName("OverlapAllDynamic");
	LeftTrackpadMarker->SetupAttachment(LeftMesh);
	LeftTrackpadMarker->SetVisibility(false);

	LeftOverlap = CreateDefaultSubobject < UStaticMeshComponent >("Left Overlap Sphere");
	LeftOverlap->SetCollisionProfileName("OverlapAllDynamic");
	LeftOverlap->SetupAttachment(LeftMesh);
	LeftOverlap->SetRelativeLocation(FVector(5.0f, 0.0f, -1.5f));

	// Right Hand - Motion Controller Component
	// Real World Controller, not the one used for gameplay
	MCCRight = CreateDefaultSubobject < UMotionControllerComponent >("MCC Right");
	MCCRight->Hand = EControllerHand::Right;
	MCCRight->SetCollisionProfileName("OverlapAllDynamic");
	MCCRight->SetupAttachment(ActorRoot);

	// Right Hand - Mesh
	// Virtual Controller, this is used for gameplay
	RightMesh = CreateDefaultSubobject < UStaticMeshComponent >("Right Mesh");
	RightMesh->SetCollisionProfileName("OverlapAllDynamic");
	RightMesh->SetupAttachment(ActorRoot);

	// Right Hand - Trackpad marker
	// Shows where on the trackpad the user is touching, if at all.
	RightTrackpadMarker = CreateDefaultSubobject < UStaticMeshComponent >("Right Touchpoint");
	RightTrackpadMarker->SetCollisionProfileName("OverlapAllDynamic");
	RightTrackpadMarker->SetupAttachment(RightMesh);
	RightTrackpadMarker->SetVisibility(false);

	RightOverlap = CreateDefaultSubobject < UStaticMeshComponent >("Right Overlap Sphere");
	RightOverlap->SetCollisionProfileName("OverlapAllDynamic");
	RightOverlap->SetupAttachment(RightMesh);
	RightOverlap->SetRelativeLocation(FVector(5.0f, 0.0f, -1.5f));



}

// Called when the game starts or when spawned
void AVive_Pawn_CPP::BeginPlay()
{
	Super::BeginPlay();

	//USteamVRFunctionLibrary::
	//USteamVRFunctionLibrary::SetTrackingSpace(ESteamVRTrackingSpace::Standing);

	// Bind Overlaps
	LeftOverlap->OnComponentBeginOverlap.AddDynamic(this, &AVive_Pawn_CPP::OnBeginOverlap_Left);
	LeftOverlap->OnComponentEndOverlap.AddDynamic(this, &AVive_Pawn_CPP::OnEndOverlap_Left);

	RightOverlap->OnComponentBeginOverlap.AddDynamic(this, &AVive_Pawn_CPP::OnBeginOverlap_Right);
	RightOverlap->OnComponentEndOverlap.AddDynamic(this, &AVive_Pawn_CPP::OnEndOverlap_Right);
}

// Called every frame
void AVive_Pawn_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Updates the location of the HMD
	FVector HMD_Location;
	FRotator HMD_Rotator;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMD_Rotator, HMD_Location);
	HMDMesh->SetWorldLocationAndRotation(GetActorLocation() + GetActorRotation().RotateVector(HMD_Location), HMD_Rotator + GetActorRotation());


	// Updates the Trackpad
	UpdateTouchpoint_Left();
	UpdateTouchpoint_Right();

	// Updates the Controller Positions
	UpdateMCPosition_Left();
	UpdateMCPosition_Right();

	// Updates trackpad stuff
	if (bInLevelSelect)
	{
		MenuScroll();
	}

	if (TimeSinceLastMove < LongTimeThreshold)
	{
		TimeSinceLastMove += DeltaTime;
	}

	if (!(MenuHeld_Left < 0))
	{
		MenuHeld_Left += DeltaTime;
		if (MenuHeld_Left >= MenuHoldQuit)
		{
			MenuDoubleTapExit();
		}

		if (MenuHeld_Left >= MenuHoldReturn)
		{
			PlayHapticFeedback(LargeIntensity, LargeDuration, true, false);
		}
	}

	if (!(MenuHeld_Right < 0))
	{
		MenuHeld_Right += DeltaTime;
		if (MenuHeld_Right >= MenuHoldQuit)
		{
			MenuDoubleTapExit();
		}

		if (MenuHeld_Right >= MenuHoldReturn)
		{
			PlayHapticFeedback(LargeIntensity, LargeDuration, false, true);
		}
	}

	if (bTestLevelSelect)
	{
		bTestLevelSelect = false;
		UE_LOG(LogTemp, Log, TEXT("Testing Level Select"));
		GoToLevelSelect();
	}

}

// Called to bind functionality to input
void AVive_Pawn_CPP::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	// Bind Pressed Actions
	// InputComponent->BindAction("Jump", IE_Pressed, this, &AGravityCharacter::Jump);
	InputComponent->BindAction("Vive_Left_Trigger", EInputEvent::IE_Pressed, this, &AVive_Pawn_CPP::LeftTrigger_Pressed);
	InputComponent->BindAction("Vive_Left_Trigger", EInputEvent::IE_Released, this, &AVive_Pawn_CPP::LeftTrigger_Released);

	InputComponent->BindAction("Vive_Right_Trigger", EInputEvent::IE_Pressed, this, &AVive_Pawn_CPP::RightTrigger_Pressed);
	InputComponent->BindAction("Vive_Right_Trigger", EInputEvent::IE_Released, this, &AVive_Pawn_CPP::RightTrigger_Released);

	InputComponent->BindAction("Vive_Left_Touchpad", EInputEvent::IE_Pressed, this, &AVive_Pawn_CPP::LeftTouchpad_Pressed);

	InputComponent->BindAction("Vive_Right_Touchpad", EInputEvent::IE_Pressed, this, &AVive_Pawn_CPP::RightTouchpad_Pressed);

	InputComponent->BindAction("Vive_Left_Menu", EInputEvent::IE_Pressed, this, &AVive_Pawn_CPP::LeftMenu_Pressed);
	InputComponent->BindAction("Vive_Left_Menu", EInputEvent::IE_Released, this, &AVive_Pawn_CPP::LeftMenu_Released);

	InputComponent->BindAction("Vive_Right_Menu", EInputEvent::IE_Pressed, this, &AVive_Pawn_CPP::RightMenu_Pressed);
	InputComponent->BindAction("Vive_Right_Menu", EInputEvent::IE_Released, this, &AVive_Pawn_CPP::RightMenu_Released);

	// Bind Axis Actions
	// InputComponent->BindAxis("Turn", this, &AElementalCharacter::ModControllerYawInput);
	InputComponent->BindAxis("Vive_Left_Trackpad_X", this, &AVive_Pawn_CPP::LeftTrackpadX_Update);
	InputComponent->BindAxis("Vive_Left_Trackpad_Y", this, &AVive_Pawn_CPP::LeftTrackpadY_Update);

	InputComponent->BindAxis("Vive_Right_Trackpad_X", this, &AVive_Pawn_CPP::RightTrackpadX_Update);
	InputComponent->BindAxis("Vive_Right_Trackpad_Y", this, &AVive_Pawn_CPP::RightTrackpadY_Update);

}

//////////////////////////////////////////////////////////////////////////
// Controlers
//////////////////////////////////////////////////////////////////////////
void AVive_Pawn_CPP::LeftTrackpadX_Update(float inVal)
{
	// Updates the X value for the left trackpad
	LeftTrackpadX = inVal;
}
void AVive_Pawn_CPP::LeftTrackpadY_Update(float inVal)
{
	// Updates the Y value for the left trackpad
	LeftTrackpadY = inVal;
}

void AVive_Pawn_CPP::UpdateTouchpoint_Left()
{
	// Updates the left trackpad touchpoint
	if ((LeftTrackpadX == 0) && (LeftTrackpadY == 0))
	{
		// Either there is no touching OR the user is magic and is perfectly centered
		LeftTrackpadMarker->SetVisibility(false);
	}
	else
	{
		// These sockets are set inside the mesh in UE4
		FVector LeftLoc = LeftMesh->GetSocketTransform("TrackpadLeft").GetLocation();
		FVector DownLoc = LeftMesh->GetSocketTransform("TrackpadDown").GetLocation();
		FVector CenterLoc = LeftMesh->GetSocketTransform("TrackpadCenter").GetLocation();

		FVector LeftDif = FMath::Lerp(FVector::ZeroVector, LeftLoc - CenterLoc, LeftTrackpadX);
		FVector DownDif = FMath::Lerp(FVector::ZeroVector, DownLoc - CenterLoc, LeftTrackpadY);

		LeftTrackpadMarker->SetWorldLocation((CenterLoc - LeftDif) - DownDif);
		LeftTrackpadMarker->SetVisibility(true);

	}
}

void AVive_Pawn_CPP::RightTrackpadX_Update(float inVal)
{
	// Updates the X value for the right trackpad
	RightTrackpadX = inVal;
}
void AVive_Pawn_CPP::RightTrackpadY_Update(float inVal)
{
	// Updates the Y value for the right trackpad
	RightTrackpadY = inVal;
}

void AVive_Pawn_CPP::UpdateTouchpoint_Right()
{
	// Updates the right trackpad touchpoint
	if ((RightTrackpadX == 0) && (RightTrackpadY == 0))
	{
		// Either there is no touching OR the user is magic and is perfectly centered
		RightTrackpadMarker->SetVisibility(false);
	}
	else
	{

		// These sockets are set inside the mesh in UE4
		FVector LeftLoc = RightMesh->GetSocketTransform("TrackpadLeft").GetLocation();
		FVector DownLoc = RightMesh->GetSocketTransform("TrackpadDown").GetLocation();
		FVector CenterLoc = RightMesh->GetSocketTransform("TrackpadCenter").GetLocation();

		FVector LeftDif = FMath::Lerp(FVector::ZeroVector, LeftLoc - CenterLoc, RightTrackpadX);
		FVector DownDif = FMath::Lerp(FVector::ZeroVector, DownLoc - CenterLoc, RightTrackpadY);

		RightTrackpadMarker->SetWorldLocation((CenterLoc - LeftDif) - DownDif);
		RightTrackpadMarker->SetVisibility(true);

	}
}

void AVive_Pawn_CPP::LeftTrigger_Pressed()
{
	if ((!ActorHeld_Left) && (bCanPickUp))
	{

		if (SOI_Left.Num() != 0)
		{
			ActorHeld_Left = SOI_Left[0];
			if (ActorHeld_Left->GetAttachParentActor() == this)
			{
				ActorHeld_Right = nullptr;
			}
			FAttachmentTransformRules FATR(EAttachmentRule::KeepWorld, true);
			ActorHeld_Left->AttachToComponent(LeftMesh, FATR);
			PlayHapticFeedback(LargeIntensity, LargeDuration, true, false);
		}

	}
}
void AVive_Pawn_CPP::LeftTrigger_Released()
{
	if (ActorHeld_Left)
	{
		PlayHapticFeedback(SmallIntensity, SmallDuration, true, false);
		ActorHeld_Left->DetachRootComponentFromParent();
		ActorHeld_Left = nullptr;
	}
}

void AVive_Pawn_CPP::RightTrigger_Pressed()
{

	if ((!ActorHeld_Right) && (bCanPickUp))
	{

		if (SOI_Right.Num() != 0)
		{
			ActorHeld_Right = SOI_Right[0];
			if (ActorHeld_Right->GetAttachParentActor() == this)
			{
				ActorHeld_Left = nullptr;
			}
			FAttachmentTransformRules FATR(EAttachmentRule::KeepWorld, true);
			ActorHeld_Right->AttachToComponent(RightMesh, FATR);
			PlayHapticFeedback(LargeIntensity, LargeDuration, false, true);
		}

	}
}
void AVive_Pawn_CPP::RightTrigger_Released()
{
	if (ActorHeld_Right)
	{
		PlayHapticFeedback(SmallIntensity, SmallDuration, false, true);
		ActorHeld_Right->DetachRootComponentFromParent();
		ActorHeld_Right = nullptr;
	}
}

void AVive_Pawn_CPP::UpdateMCPosition_Left()
{
	// Update the position and rotation of the fake left controller with input from the real one.

	// LeftMesh is the visible virtual left controller.
	// This is the one used for gameplay and gets smoothed.
	FVector MeshPos = LeftMesh->GetComponentLocation();
	FRotator MeshRot = LeftMesh->GetComponentRotation();

	// MCCLeft is the Motion Controller Component for the left controller.
	// It is the unmodified data from the real-world controller.
	// This one is hidden and not used for gameplay.
	FVector MCCPos = MCCLeft->GetComponentLocation();
	FRotator MCCRot = MCCLeft->GetComponentRotation();

	// This is the smoothing algorithm from Nathan (nathan@sculptrvr.com)
	// It takes the difference between the position of the LeftMesh ( Game Controller ) and MCCLeft ( Real Controller )
	// It then takes the magnitude of the difference vector and divides it by three.
	// Finally it returns either one third the magnitude or one, depending on which is smaller.

	// If sensitivity needs to change, alter the three.
	float AlphaValue = FMath::Min((((MeshPos - MCCPos).Size()) / 3), 1.0f);

	// This updates the location and rotation of LeftMesh ( Game Controller ) to the values of the MCCLeft ( Real Controller )
	// It uses the alpha value calculated above to interpolate both the vector and rotation.
	// The alpha value is used to blend the two. It is bound between 0 and 1. 0 is LeftMesh and 1 is MCCLeft.
	LeftMesh->SetWorldLocationAndRotation(FMath::VInterpTo(MeshPos, MCCPos, AlphaValue, 1), FMath::RInterpTo(MeshRot, MCCRot, AlphaValue, 1));
}
void AVive_Pawn_CPP::UpdateMCPosition_Right()
{
	// Update the position and rotation of the fake right controller with input from the real one.
	// If I was smart, I could have combined these into one function

	// RightMesh is the visible virtual right controller.
	// This is the one used for gameplay and gets smoothed.
	FVector MeshPos = RightMesh->GetComponentLocation();
	FVector MCCPos = MCCRight->GetComponentLocation();

	// MCCRight is the Motion Controller Component for the right controller.
	// It is the unmodified data from the real-world controller.
	// This one is hidden and not used for gameplay.
	FRotator MeshRot = RightMesh->GetComponentRotation();
	FRotator MCCRot = MCCRight->GetComponentRotation();

	// This is the smoothing algorithm from Nathan (nathan@sculptrvr.com)
	// It takes the difference between the position of the LeftMesh ( Game Controller ) and MCCLeft ( Real Controller )
	// It then takes the magnitude of the difference vector and divides it by three.
	// Finally it returns either one third the magnitude or one, depending on which is smaller.

	// If sensitivity needs to change, alter the three.
	float AlphaValue = FMath::Min((((MeshPos - MCCPos).Size()) / 3), 1.0f);

	// This updates the location and rotation of RightMesh ( Game Controller ) to the values of the MCCRight ( Real Controller )
	// It uses the alpha value calculated above to interpolate both the vector and rotation.
	// The alpha value is used to blend the two. It is bound between 0 and 1. 0 is RightMesh and 1 is MCCRight.
	RightMesh->SetWorldLocationAndRotation(FMath::VInterpTo(MeshPos, MCCPos, AlphaValue, 1), FMath::RInterpTo(MeshRot, MCCRot, AlphaValue, 1));
}

void AVive_Pawn_CPP::LeftMenu_Pressed()
{
	MenuHeld_Left = 0.0f;
	PlayHapticFeedback(SmallIntensity, SmallDuration, true, false);
}
void AVive_Pawn_CPP::RightMenu_Pressed()
{
	MenuHeld_Right = 0.0f;
	PlayHapticFeedback(SmallIntensity, SmallDuration, false, true);
}

void AVive_Pawn_CPP::LeftMenu_Released()
{
	if (MenuHeld_Left >= MenuHoldReturn)
	{
		GoToLevelSelect();
	}

	MenuHeld_Left = -1;
}
void AVive_Pawn_CPP::RightMenu_Released()
{
	if (MenuHeld_Right >= MenuHoldReturn)
	{
		GoToLevelSelect();
	}

	MenuHeld_Right = -1;
}

void AVive_Pawn_CPP::LeftTouchpad_Pressed()
{
	if (bInLevelSelect)
	{
		MenuTouchpad(true, false);
	}
}
void AVive_Pawn_CPP::RightTouchpad_Pressed()
{
	if (bInLevelSelect)
	{
		MenuTouchpad(false, true);
	}
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void AVive_Pawn_CPP::OnBeginOverlap_Left(class UPrimitiveComponent* Comp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<IVR_Movable_Interface>(OtherActor))
	{
		SOI_Left.AddUnique(OtherActor);
	}
}
void AVive_Pawn_CPP::OnEndOverlap_Left(class UPrimitiveComponent* Comp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	SOI_Left.RemoveSingle(OtherActor);
}

void AVive_Pawn_CPP::OnBeginOverlap_Right(class UPrimitiveComponent* Comp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<IVR_Movable_Interface>(OtherActor))
	{
		SOI_Right.AddUnique(OtherActor);
	}
}
void AVive_Pawn_CPP::OnEndOverlap_Right(class UPrimitiveComponent* Comp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	SOI_Right.RemoveSingle(OtherActor);
}

void AVive_Pawn_CPP::SetPickUp(bool PickUp)
{
	bCanPickUp = PickUp;
}

void AVive_Pawn_CPP::DropAllHeld()
{
	if (ActorHeld_Left)
	{
		ActorHeld_Left->DetachRootComponentFromParent();
		ActorHeld_Left = nullptr;
	}

	if (ActorHeld_Right)
	{
		ActorHeld_Right->DetachRootComponentFromParent();
		ActorHeld_Right = nullptr;
	}
}

void AVive_Pawn_CPP::GoToLevelSelect_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("'GoToLevelSelect()' NEEDS TO BE IMPLEMENTED IN BLUEPRINT"));
}

void AVive_Pawn_CPP::GoToLevel_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("'GoToLevel()' NEEDS TO BE IMPLEMENTED IN BLUEPRINT"));
}

void AVive_Pawn_CPP::MenuDoubleTapExit_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("'MenuDoubleTapExit()' NEEDS TO BE IMPLEMENTED IN BLUEPRINT"));
}

void AVive_Pawn_CPP::PlayHapticFeedback_Implementation(float Intensity, float Duration, bool bLeft, bool bRight)
{
	UE_LOG(LogTemp, Error, TEXT("'PlayHapticFeedback()' NEEDS TO BE IMPLEMENTED IN BLUEPRINT"));
}

void AVive_Pawn_CPP::SetMonitor(ALevelSelectMonitor* NewMonitor)
{
	LevelMonitor = NewMonitor;
	if (NewMonitor)
	{
		bInLevelSelect = true;
	}
	else
	{
		bInLevelSelect = false;
	}
}

void AVive_Pawn_CPP::MenuTouchpad(bool bLeft, bool bRight)
{
	if (bInLevelSelect)
	{
		LevelMonitor->UpdateMonitor();
		float XAvg, YAvg;
		if (bLeft)
		{
			XAvg = LeftTrackpadX;
			YAvg = LeftTrackpadY;
		}
		else
		{
			XAvg = RightTrackpadX;
			YAvg = RightTrackpadY;
		}

		if ((FMath::Abs(XAvg) > TrackpadThreshold) || (FMath::Abs(YAvg) > TrackpadThreshold))
		{
			if (FMath::Abs(XAvg) > FMath::Abs(YAvg))
			{
				if (XAvg > 0)
				{
					PlayHapticFeedback(LargeIntensity, LargeDuration, bLeft, bRight);
					LevelMonitor->ForwardClickRow();
				}
				else
				{
					PlayHapticFeedback(SmallIntensity, SmallDuration, bLeft, bRight);
					LevelMonitor->BackwardClickRow();
				}
			}
			else
			{
				if (YAvg > 0)
				{
					PlayHapticFeedback(SmallIntensity, SmallDuration, bLeft, bRight);
					LevelMonitor->DecrementRow();
				}
				else
				{
					PlayHapticFeedback(SmallIntensity, SmallDuration, bLeft, bRight);
					LevelMonitor->IncrementRow();
				}
			}

		}
	}
}