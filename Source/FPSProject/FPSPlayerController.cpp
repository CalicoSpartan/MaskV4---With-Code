// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerController.h"
#include "FPSProjectGameModeBase.h"
#include "Net/UnrealNetwork.h"


void AFPSPlayerController::OnKilled()
{

	UnPossess();
	UE_LOG(LogClass, Log, TEXT("should respawn"));
	GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &AFPSPlayerController::Respawn, 5.0f);

}


void AFPSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSPlayerController, TeamNumber);
	DOREPLIFETIME(AFPSPlayerController, Score);
	DOREPLIFETIME(AFPSPlayerController, Kills);
	DOREPLIFETIME(AFPSPlayerController, Deaths);
	DOREPLIFETIME(AFPSPlayerController, Assists);



}

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	MyPlayerState = Cast<AFPSPlayerState>(PlayerState);
	if (MyPlayerState)
	{
		UE_LOG(LogClass, Log, TEXT("MyPlayerState Created"));
	}
}
void AFPSPlayerController::ChangeKills(int32 delta)
{
	Kills += delta;
}
void AFPSPlayerController::ChangeScore(int32 delta)
{
	Score += delta;
}

void AFPSPlayerController::ChangeDeaths(int32 delta)
{
	Deaths += delta;
}

int32 AFPSPlayerController::GetDeaths()
{
	return Deaths;
}
int32 AFPSPlayerController::GetScore()
{
	return Score;
}
int32 AFPSPlayerController::GetKills()
{
	return Kills;
}

void AFPSPlayerController::Respawn()
{

	AGameModeBase * GameMode = GetWorld()->GetAuthGameMode();
	if (GameMode)
	{

		ServerRespawnPlayer();
	}

}


void AFPSPlayerController::ServerSetPlayerTeam_Implementation(int32 NewTeam)
{
	TeamNumber = NewTeam;
	ServerSpawnPlayer();
}

bool AFPSPlayerController::ServerSetPlayerTeam_Validate(int32 NewTeam)
{
	return true;
}

void AFPSPlayerController::ServerSpawnPlayer_Implementation()
{
	// Spawn the new player
	AFPSProjectGameModeBase * GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->StartNewPlayer(this);
	}

}

bool AFPSPlayerController::ServerSpawnPlayer_Validate()
{
	return true;
}
void AFPSPlayerController::ServerRespawnPlayer_Implementation()
{
	// Spawn the new player
	AFPSProjectGameModeBase * GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->RespawnPlayer(this);
	}

}

bool AFPSPlayerController::ServerRespawnPlayer_Validate()
{
	return true;
}


void AFPSPlayerController::ServerSetPlayerTeamClient_Implementation(int32 NewTeam)
{
	TeamNumber = NewTeam;
	ServerSpawnPlayerClient();
}

bool AFPSPlayerController::ServerSetPlayerTeamClient_Validate(int32 NewTeam)
{
	return true;
}

void AFPSPlayerController::ServerSpawnPlayerClient_Implementation()
{
	// Spawn the new player
	AFPSProjectGameModeBase * GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->StartNewPlayerClient(this);
	}

}

bool AFPSPlayerController::ServerSpawnPlayerClient_Validate()
{
	return true;
}


int32 AFPSPlayerController::GetPlayerTeam()
{
	return TeamNumber;
}

void AFPSPlayerController::TriggerAddAliveUI_Implementation()
{
	TriggerAddAliveUIBlueprint();

}
bool AFPSPlayerController::TriggerAddAliveUI_Validate() {
	return true;
}
void AFPSPlayerController::TriggerAddAliveUIBlueprint_Implementation()
{

}
void AFPSPlayerController::AddScoreBoardUI_Implementation()
{
	AddScoreBoardUIBlueprint();

}
bool AFPSPlayerController::AddScoreBoardUI_Validate() {
	return true;
}
void AFPSPlayerController::AddScoreBoardUIBlueprint_Implementation()
{

}

void AFPSPlayerController::UpdateScoreBoardUI_Implementation()
{
	UpdateScoreBoardUIBlueprint();

}
bool AFPSPlayerController::UpdateScoreBoardUI_Validate() {
	return true;
}
void AFPSPlayerController::UpdateScoreBoardUIBlueprint_Implementation()
{

}
void AFPSPlayerController::UpdateScoreUI_Implementation()
{
	UpdateScoreUIBlueprint();

}
bool AFPSPlayerController::UpdateScoreUI_Validate() {
	return true;
}
void AFPSPlayerController::UpdateScoreUIBlueprint_Implementation()
{

}

void AFPSPlayerController::Tick(float DeltaTime)
{
	UpdateScoreUI();

}

bool AFPSPlayerController::UpdateRivalStats_Validate(int32 RivalTeam, int32 RivalScore, float RivalPercent)
{
	return true;
}

void AFPSPlayerController::UpdateRivalStats_Implementation(int32 RivalTeam, int32 RivalScore, float RivalPercent)
{
	RivalTeamNumber = RivalTeam;
	RivalTeamScore = RivalScore;
	RivalScorePercent = RivalPercent;

}
bool AFPSPlayerController::UpdateMyTeamStats_Validate(int32 NewScore, float MyPercent)
{
	return true;
}

void AFPSPlayerController::UpdateMyTeamStats_Implementation(int32 NewScore, float MyPercent)
{
	MyTeamScore = NewScore;
	TeamScorePercent = MyPercent;


}