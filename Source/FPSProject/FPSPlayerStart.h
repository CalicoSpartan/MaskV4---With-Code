// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "FPSPlayerStart.generated.h"

/**
*
*/
UCLASS()
class FPSPROJECT_API AFPSPlayerStart : public APlayerStart
{
	GENERATED_BODY()


public:


	UPROPERTY(EditAnywhere, Category = "Defaults")
		bool bTeamB;

	UPROPERTY(EditAnywhere, Category = "Defaults")
		int32 TeamNumber;

	UPROPERTY(VisibleAnywhere)
		UShapeComponent* TakenBox;

};
