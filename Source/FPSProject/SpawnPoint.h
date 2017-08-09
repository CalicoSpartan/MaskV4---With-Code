// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "SpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API ASpawnPoint : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	ASpawnPoint();
	UFUNCTION(BlueprintPure)
		bool IsActive();
	
	//allows other classes to safely change the spawnpoint's activation state
	UFUNCTION(BlueprintCallable)
		void SetActive(bool NewPointState);

	UPROPERTY(EditDefaultsOnly)
		bool IsBlocked;

	//required network scaffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	//true when the pickup can be used, false when the pickup is deactivated
	UPROPERTY(Replicated = OnRep_IsActive)
		bool bIsActive;
	//this is called whenever bIsActive is updated
	UFUNCTION()
		virtual void OnRep_IsActive();
	
};
