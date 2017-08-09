// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerState.h"
#include "FPSPlayerController.generated.h"

/**
*
*/
UCLASS()
class FPSPROJECT_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

		FTimerHandle TimerHandle_Respawn;

public:

	UFUNCTION(Server, Reliable, WithValidation)
		void TriggerAddAliveUI();
	virtual void TriggerAddAliveUI_Implementation();
	virtual bool TriggerAddAliveUI_Validate();
	UFUNCTION(Server, Reliable, WithValidation)
		void AddScoreBoardUI();
	virtual void AddScoreBoardUI_Implementation();
	virtual bool AddScoreBoardUI_Validate();
	UFUNCTION(BlueprintNativeEvent)
		void AddScoreBoardUIBlueprint();
	UFUNCTION(BlueprintNativeEvent)
		void TriggerAddAliveUIBlueprint();
	UFUNCTION(Server, Reliable, WithValidation)
		void UpdateScoreUI();
	virtual void UpdateScoreUI_Implementation();
	virtual bool UpdateScoreUI_Validate();
	UFUNCTION(Client, Reliable, WithValidation)//Server, Reliable, WithValidation)
		void UpdateRivalStats(int32 RivalTeam, int32 RivalScore, float RivalPercent);
	UFUNCTION(Client, Reliable, WithValidation)//Server, Reliable, WithValidation)
		void UpdateMyTeamStats(int32 NewScore, float MyPercent);
	UFUNCTION(BlueprintNativeEvent)
		void UpdateScoreUIBlueprint();
	UFUNCTION(Server, Reliable, WithValidation)
		void UpdateScoreBoardUI();
	virtual void UpdateScoreBoardUI_Implementation();
	virtual bool UpdateScoreBoardUI_Validate();
	UFUNCTION(BlueprintNativeEvent)
		void UpdateScoreBoardUIBlueprint();

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface")
		int32 MyTeamScore = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface")
		int32 RivalTeamNumber = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface")
		float RivalScorePercent = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface")
		int32 RivalTeamScore = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface")
		float TeamScorePercent = 0.0f;



	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawnPlayer();
	virtual void ServerSpawnPlayer_Implementation();
	virtual bool ServerSpawnPlayer_Validate();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawnPlayerClient();
	virtual void ServerSpawnPlayerClient_Implementation();
	virtual bool ServerSpawnPlayerClient_Validate();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Info")
		void ServerSetPlayerTeam(int32 NewTeam);
	virtual void ServerSetPlayerTeam_Implementation(int32 NewTeam);
	virtual bool ServerSetPlayerTeam_Validate(int32 NewTeam);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Info")
		void ServerSetPlayerTeamClient(int32 NewTeam);
	virtual void ServerSetPlayerTeamClient_Implementation(int32 NewTeam);
	virtual bool ServerSetPlayerTeamClient_Validate(int32 NewTeam);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Spawning")
		void ServerRespawnPlayer();
	virtual void ServerRespawnPlayer_Implementation();
	virtual bool ServerRespawnPlayer_Validate();



	UFUNCTION(BlueprintCallable, Category = "Interface")
		int32 GetPlayerTeam();
	UFUNCTION(BlueprintCallable, Category = "Interface")
		int32 GetScore();

	void OnKilled();

	void Respawn();
	UPROPERTY(VisibleAnywhere, Category = "PlayerState")
		AFPSPlayerState* MyPlayerState;

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		void ChangeDeaths(int32 delta);
	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		void ChangeKills(int32 delta);
	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		void ChangeScore(int32 delta);
	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		int32 GetKills();
	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
		int32 GetDeaths();

private:
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Team")
		int32 TeamNumber = 0;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "PlayerStats")
		int32 Kills = 0;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "PlayerStats")
		int32 Deaths = 0;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "PlayerStats")
		int32 Assists = 0;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "PlayerStats")
		int32 Score = 0;

};
