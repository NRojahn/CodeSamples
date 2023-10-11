

#include "LightRepairTeam4.h"
#include "Vive_Pawn_CPP.h"
#include "LRT4SaveGame.h"
#include "LRT4_LevelScriptActor_CPP.h"

// Called every frame
void ALRT4_LevelScriptActor_CPP::Tick(float DeltaTime)
{
	// Current State of the Level
	switch (LevelState)
	{
		// 0 - Fade-In
	case 0:
		StateFadeIn(DeltaTime);
		break;

		// 2 - Level Completed
	case 2:
		StateCompleted(DeltaTime);
		break;

		// 3 - Lights On
	case 3:
		StateLights(DeltaTime);
		break;

		// 4 - Fade-Out
	case 4:
		StateFadeOut(DeltaTime);
		break;

		// 5 - Load Next Level
	case 5:
		StateNextLevel(DeltaTime);
		break;


		// 1 - Gameplay
	default:
		StateGameplay(DeltaTime);
		break;
	}
}

void ALRT4_LevelScriptActor_CPP::StateFadeIn(float DeltaTime)
{
	if (StateTimer == 0.0f)
	{
		// First time in this state
		FadeIn();
		UE_LOG(LogTemp, Warning, TEXT("StateFadeIn"));

	}

	StateTimer += DeltaTime;

	if (StateTimer >= SequenceTime)
	{
		StateTimer = 0.0f;
		LevelState++;
	}
}
void ALRT4_LevelScriptActor_CPP::StateGameplay(float DeltaTime)
{
	if (StateTimer == 0.0f)
	{
		// First time in this state
		UE_LOG(LogTemp, Warning, TEXT("StateGameplay"));
		// Unlock Moving Items
		if (AVive_Pawn_CPP* VivePawn = Cast<AVive_Pawn_CPP>(GWorld.GetReference()->GetFirstPlayerController()->GetPawn()))
		{
			VivePawn->SetPickUp(true);
		}
		StateTimer += DeltaTime;
		
	}

	PlayerTime += DeltaTime;
	if (PlayerTime > 6000.0f)
	{
		PlayerTime = 6000.0f;
	}
	
	bool bLevelDone = true;
	for (int i = 0; (i < Recievers.Num()) && (bLevelDone); i++)
	{
		bLevelDone = (bLevelDone) && (Recievers[i]->isCharged());
	}

	if (bLevelDone)
	{
		StateTimer = 0.0f;
		LevelState++;
	}
}

void ALRT4_LevelScriptActor_CPP::StateCompleted(float DeltaTime)
{
	if (StateTimer == 0.0f)
	{
		// First time in this state
		UE_LOG(LogTemp, Warning, TEXT("StateCompleted"));
		// Lock Moving Items
		if (AVive_Pawn_CPP* VivePawn = Cast<AVive_Pawn_CPP>(GWorld.GetReference()->GetFirstPlayerController()->GetPawn()))
		{
			VivePawn->SetPickUp(false);
			VivePawn->DropAllHeld();
		}
	}

	StateTimer += DeltaTime;
	if (StateTimer >= SequenceTime)
	{
		StateTimer = 0.0f;
		LevelState++;
	}
}

void ALRT4_LevelScriptActor_CPP::StateLights(float DeltaTime)
{
	if (StateTimer == 0.0f)
	{
		// First time in this state
		UE_LOG(LogTemp, Warning, TEXT("StateLights"));

		if (AtLightingSet < LightSets.Num())
		{
			for (int i = 0; i < LightSets[AtLightingSet].LightsInSet.Num(); i++)
			{
				LightSets[AtLightingSet].LightsInSet[i]->GetStaticMeshComponent()->SetMaterial(1, LightSets[AtLightingSet].LightMaterial);
			}

			StateTimer += DeltaTime;
		}
		else
		{
			StateTimer = 0.0f;
			LevelState++;
		}
	}
	else if (StateTimer >= LightSets[AtLightingSet].DelayTime)
	{
		StateTimer = 0.0f;
		AtLightingSet++;
	}
	else
	{
		StateTimer += DeltaTime;
	}
}

void ALRT4_LevelScriptActor_CPP::StateFadeOut(float DeltaTime)
{
	if (StateTimer == 0.0f)
	{
		// First time in this state
		UE_LOG(LogTemp, Warning, TEXT("StateFadeOut"));
		FadeOut();
	}

	StateTimer += DeltaTime;

	if (StateTimer >= SequenceTime)
	{
		StateTimer = 0.0f;
		LevelState++;
	}
}


void ALRT4_LevelScriptActor_CPP::StateNextLevel(float DeltaTime)
{
	if (StateTimer == 0.0f)
	{
		// First time in this state
		UE_LOG(LogTemp, Warning, TEXT("StateNextLevel"));
		StateTimer += DeltaTime;
		
		LoadNextLevel();
	}

}

void ALRT4_LevelScriptActor_CPP::SaveTime(FName LevelName)
{
	// LevelName.ToString()
	ULRT4SaveGame* SaveFile = ULRT4SaveGame::GetSaveGame();
	if (SaveFile->LevelTimes.Contains(LevelName.ToString()))
	{
		if (*SaveFile->LevelTimes.Find(LevelName.ToString()) > PlayerTime)
		{
			*SaveFile->LevelTimes.Find(LevelName.ToString()) = PlayerTime;
		}
	}
	else
	{
		SaveFile->LevelTimes.Add(LevelName.ToString(), PlayerTime);
	}

	SaveFile->SaveTheFile();
}