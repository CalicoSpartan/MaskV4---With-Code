// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerState.h"
#include "Net/UnrealNetwork.h"

void AFPSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSPlayerState, bTeamB);
	DOREPLIFETIME(AFPSPlayerState, TeamNumber);
	DOREPLIFETIME(AFPSPlayerState, MyScore);
	DOREPLIFETIME(AFPSPlayerState, UserName);
	DOREPLIFETIME(AFPSPlayerState, Kills);
	DOREPLIFETIME(AFPSPlayerState, Deaths);
	DOREPLIFETIME(AFPSPlayerState, Assists);
	DOREPLIFETIME(AFPSPlayerState, Team);
}

void AFPSPlayerState::SetTeam_Implementation(class ABaseTeam* MyTeam)
{
	Team = MyTeam;
}

void AFPSPlayerState::SetUserNameMultiCast_Implementation(FName NewName)
{
	//UserName = "PENIS";
	UserName = NewName.ToString();
}
void AFPSPlayerState::SetScoreMultiCast_Implementation(int32 delta)
{
	MyScore += delta;
}

void AFPSPlayerState::SetKillsMultiCast_Implementation(int32 changekills)
{
	Kills += changekills;
}
void AFPSPlayerState::SetDeathsMultiCast_Implementation(int32 delta)
{
	Deaths += delta;
}
void AFPSPlayerState::SetAssistsMultiCast_Implementation(int32 delta)
{
	Assists += delta;
}

FString AFPSPlayerState::GetUserName()
{
	return UserName;
}

int32 AFPSPlayerState::GetScore()
{
	return MyScore;
}
int32 AFPSPlayerState::GetKills()
{
	return Kills;
}
int32 AFPSPlayerState::GetDeaths()
{
	return Deaths;
}
int32 AFPSPlayerState::GetAssists()
{
	return Assists;
}