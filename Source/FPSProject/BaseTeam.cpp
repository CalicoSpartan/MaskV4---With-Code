// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseTeam.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ABaseTeam::ABaseTeam()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

void ABaseTeam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseTeam, TeamPlayerStates);
	DOREPLIFETIME(ABaseTeam, TeamColor);
	DOREPLIFETIME(ABaseTeam, TeamName);
	DOREPLIFETIME(ABaseTeam, TeamNumber);
}

bool ABaseTeam::UpdateTeamOrder_Validate(const TArray<AFPSPlayerState*>& NewOrder)
{
	return true;
}
void ABaseTeam::UpdateTeamOrder_Implementation(const TArray<AFPSPlayerState*>& NewOrder)
{
	TeamPlayerStates.Empty();
	TeamPlayerStates = NewOrder;
}

// Called when the game starts or when spawned
void ABaseTeam::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseTeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

