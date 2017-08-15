// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTeam.h"
#include "GameFramework/PlayerState.h"
#include "FPSPlayerState.generated.h"

/**
*
*/
UCLASS()
class FPSPROJECT_API AFPSPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Team")
		bool bTeamB;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Team")
		int32 TeamNumber = 0;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Team")
		FString UserName = "POOOOOOP";
	//required network scaffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable, Category = "Stats")
		FString GetUserName();
	UFUNCTION(BlueprintCallable, Category = "Stats")
		int32 GetScore();
	UFUNCTION(BlueprintCallable, Category = "Stats")
		int32 GetKills();
	UFUNCTION(BlueprintCallable, Category = "Stats")
		int32 GetDeaths();
	UFUNCTION(BlueprintCallable, Category = "Stats")
		int32 GetAssists();
	UFUNCTION(NetMultiCast, Reliable, Category = "Stats")
		void SetScoreMultiCast(int32 delta);
	UFUNCTION(NetMultiCast, Reliable, Category = "Stats")
		void SetUserNameMultiCast(FName NewName);
	UFUNCTION(NetMultiCast, Reliable, Category = "Stats")
		void SetKillsMultiCast(int32 delta);
	UFUNCTION(NetMultiCast, Reliable, Category = "Stats")
		void SetDeathsMultiCast(int32 delta);
	UFUNCTION(NetMultiCast, Reliable, Category = "Stats")
		void SetAssistsMultiCast(int32 delta);
	UFUNCTION(NetMultiCast, Reliable, Category = "Team")
		void SetTeam(class ABaseTeam* MyTeam);


	UPROPERTY(Replicated,EditAnywhere, Category = "Team")
		class ABaseTeam* Team;


private:

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Stats")
		int32 MyScore = 0;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Stats")
		int32 Kills = 0;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Stats")
		int32 Deaths = 0;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Stats")
		int32 Assists = 0;
};
