#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PlayerHUD.h"
#include "FPSPlayerState.h"
#include "FPSCharacter.h"
#include "FPSProjectGameModeBase.h"
#include "FPSPlayerController.h"
#include "EngineUtils.h"
#include "SpawnPoint.h"
#include "FPSGameState.generated.h"

UENUM(BlueprintType)
enum EGamePlayState
{
	EWaiting,
	EPlaying,
	EGameOver,
	EUnknown
};


/**
*
*/
UCLASS()
class FPSPROJECT_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AFPSGameState();

	virtual void BeginPlay() override;


	UPROPERTY(Replicated, BlueprintReadWrite, Category = "KillFeed")
		FString KillFeedMessage;

	UPROPERTY(BlueprintReadWrite)
		FString LastKilled;

	UPROPERTY(BlueprintReadWrite)
		FString LastKiller;



	UFUNCTION(BlueprintCallable)
		void SetNewKillFeedMessage(FString Message);
	UFUNCTION(BlueprintImplementableEvent)
		void CallHUDUpdate();
	UFUNCTION(BlueprintImplementableEvent)
		void CallHUDGameOver();
	UFUNCTION(NetMultiCast, Reliable)
		void ClientUpdateKillFeedMessage(FName text);
	UFUNCTION(NetMultiCast, Reliable)
		void ClientUpdateWinningTeam(int32 team, FName WinningText);

	UFUNCTION(BlueprintImplementableEvent)
		void BPAllowTeamChoice();
	//required network scaffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//this returns the current state of gameplay
	UFUNCTION(BlueprintPure)
		EGamePlayState GetCurrentState() const;
	//transition game into a new play state, Only usable on server
	void SetCurrentState(EGamePlayState NewState);

	// rep notify fired on clients to allow for client side changes in gameplay state
	UFUNCTION()
		void OnRep_CurrentState();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Interface")
		void UpdateTeamScore(int32 Team);
	virtual void UpdateTeamScore_Implementation(int32 Team);
	virtual bool UpdateTeamScore_Validate(int32 Team);

	// name of player who won the game
	UPROPERTY(Replicated, BlueprintReadWrite)
		FString WinningPlayerName;

	//how many times per second to update the game's UI and State
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
		float UpdateDelay;
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateScoreBlueprintEvent();
	UFUNCTION(NetMultiCast, Reliable)
		void ClientUpdateScore(int32 Team, int32 delta);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void ClientUpdateNumberOfTeams(int32 Teams);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void ClientUpdateTeam2Controllers(AFPSPlayerController* FPSController);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void ClientUpdateTeam1Controllers(AFPSPlayerController* FPSController);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void ClientUpdateTeam2PlayerStates(AFPSPlayerState* PlayerState);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void ClientUpdateTeam1PlayerStates(AFPSPlayerState* PlayerState);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void UpdateTeamOrder(const TArray<AFPSPlayerState*>& Team1Order, const TArray<AFPSPlayerState*>& Team2Order);
	UFUNCTION()
		void CallUpdateScoreboard();

	UFUNCTION(NetMulticast, Reliable)
		void SetNumberOfPlayers(int32 newNumberOfPlayers);
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		int32 NumberOfTeams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		int32 Team1Players;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		int32 Team2Players;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		TArray<AFPSPlayerState*> Team1PlayerStates;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		TArray<AFPSPlayerState*> Team2PlayerStates;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		TArray<AFPSPlayerController*> Team1PlayerControllers;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		TArray<AFPSPlayerController*> Team2PlayerControllers;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		TArray<AFPSPlayerState*> Team1PlayerOrder;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		TArray<AFPSPlayerState*> Team2PlayerOrder;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		TArray<AFPSPlayerController*> Team1CorrectOrder;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		TArray<AFPSPlayerController*> Team2CorrectOrder;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		bool CanUpdateScoreBoard = true;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		TArray<AFPSPlayerController*> Team1PlayerObjects;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameStats")
		TArray<AFPSPlayerController*> Team2PlayerObjects;

	UFUNCTION()
		void OnRep_NumberOfTeams();
	UFUNCTION(BlueprintCallable)
		int32 GetTeam1Score();
	UFUNCTION(BlueprintCallable)
		int32 GetTeam2Score();
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Stats")
		int32 Team1Score = 0;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Stats")
		int32 Team2Score = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<int32> TeamScores;

	FTimerHandle UpdateTimer;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "GameRules")
		int32 WinningTeam;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "GameRules")
		FString WinningMessage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameRules")
		AFPSPlayerController* testcontroller;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "GameRules")
		AFPSPlayerState* testplayerstate;
private:
	UPROPERTY(Replicated, EditAnywhere, Category = "Stats")
		int32 NumberOfPlayers;
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "GameRules")
		int32 ScoreToWin;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Interface")
		void ServerAllowTeamChoice();
	virtual void ServerAllowTeamChoice_Implementation();
	virtual bool ServerAllowTeamChoice_Validate();






	float QueueTimerTime;

	FTimerHandle QueueTimer;

	void BeginMatch();

	void Update();
	// track the current playing state
	UPROPERTY(ReplicatedUsing = OnRep_CurrentState)
		TEnumAsByte<enum EGamePlayState> CurrentState;


};

