// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnPoint.h"
#include "Net/UnrealNetwork.h"


ASpawnPoint::ASpawnPoint()
{
	bReplicates = true;

	PrimaryActorTick.bCanEverTick = false;
	if (Role == ROLE_Authority) {
		bIsActive = true;
	}
}

void ASpawnPoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASpawnPoint, bIsActive);
}

bool ASpawnPoint::IsActive()
{
	return bIsActive;
}

void ASpawnPoint::SetActive(bool NewPointState)
{
	if (Role == ROLE_Authority)
	{
		bIsActive = NewPointState;
	}
}

void ASpawnPoint::OnRep_IsActive()
{

}