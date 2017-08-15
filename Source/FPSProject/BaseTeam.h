// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSPlayerState.h"
#include "GameFramework/Actor.h"
#include "BaseTeam.generated.h"

UCLASS()
class FPSPROJECT_API ABaseTeam : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseTeam();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Players")
	FColor TeamColor;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Players")
	int32 TeamNumber;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Players")
	FName TeamName;
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void UpdateTeamOrder(const TArray<AFPSPlayerState*>& NewOrder);
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Players")
	TArray<AFPSPlayerState*> TeamPlayerStates;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
