#include "FPSGameState.h"
#include "Net/UnrealNetwork.h"

AFPSGameState::AFPSGameState()
{
	CurrentState = EGamePlayState::EUnknown;
	KillFeedMessage = TEXT("WHATSUPP");
	QueueTimerTime = 2.0f;
	NumberOfTeams = 0;
	TeamScores.Add(Team1Score);
	TeamScores.Add(Team2Score);

}
void AFPSGameState::OnRep_NumberOfTeams()
{

}
void AFPSGameState::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	check(World);
	CurrentState = EGamePlayState::EWaiting;
	if (CurrentState == EGamePlayState::EPlaying)
	{
		/*
		for (FConstControllerIterator controller = World->GetControllerIterator(); controller; ++controller) {
		if (AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*controller)) {
		if (PlayerController->GetPlayerTeam() == 1)
		{
		Team1PlayerControllers.AddUnique(PlayerController);
		}
		if (PlayerController->GetPlayerTeam() == 2)
		{
		Team2PlayerControllers.AddUnique(PlayerController);
		}
		}
		}
		*/
	}
	switch (GetCurrentState())
	{
	case EGamePlayState::EWaiting:
		GetWorldTimerManager().SetTimer(QueueTimer, this, &AFPSGameState::ServerAllowTeamChoice, QueueTimerTime, false);
		break;
	case EGamePlayState::EPlaying:

		//start draining power
		GetWorldTimerManager().SetTimer(UpdateTimer, this, &AFPSGameState::Update, UpdateDelay, true);
		break;
	case EGamePlayState::EGameOver:

		//stop draining power
		//GetWorldTimerManager().ClearTimer(UpdateTimer);
		break;
	default:
	case EGamePlayState::EUnknown:
		break;
	}


}

bool AFPSGameState::ServerAllowTeamChoice_Validate()
{
	return true;
}


void AFPSGameState::CallUpdateScoreboard()
{
	if (Team1PlayerOrder.Num() != 0 && Team2PlayerOrder.Num() != 0)
	{
		for (FConstControllerIterator controller = GetWorld()->GetControllerIterator(); controller; ++controller) {
			if (AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*controller)) {
				//PlayerController->UpdateScoreBoardUI();
			}
		}
		/*
		for (int32 i = 0; i < Team2PlayerOrder.Num(); ++i)
		{
		if (AFPSPlayerController* playercontroller = Cast<AFPSPlayerController>(Team2PlayerOrder[i]))
		{
		playercontroller->UpdateScoreBoardUI();
		}
		}
		for (int32 it = 0; it < Team1PlayerOrder.Num(); ++it)
		{
		if (AFPSPlayerController* playercontroller = Cast<AFPSPlayerController>(Team1PlayerOrder[it]))
		{
		playercontroller->UpdateScoreBoardUI();
		}
		}
		*/
	}
}
void AFPSGameState::ServerAllowTeamChoice_Implementation()
{
	if (Role == ROLE_Authority)
	{
		BPAllowTeamChoice();
	}
}
void AFPSGameState::ClientUpdateScore_Implementation(int32 Team, int32 delta)
{
	if (Team == 1)
	{
		Team1Score += delta;
	}
	if (Team == 2)
	{
		Team2Score += delta;
	}
}
void AFPSGameState::ClientUpdateKillFeedMessage_Implementation(FName text)
{

	KillFeedMessage = text.ToString();

}
bool AFPSGameState::ClientUpdateNumberOfTeams_Validate(int32 Teams)
{
	return true;
}

void AFPSGameState::ClientUpdateNumberOfTeams_Implementation(int32 Teams)
{

	NumberOfTeams = Teams;

}





bool AFPSGameState::UpdateTeamOrder_Validate(const TArray<AFPSPlayerState*>& Team1Order, const TArray<AFPSPlayerState*>& Team2Order)
{
	return true;
}
void AFPSGameState::UpdateTeamOrder_Implementation(const TArray<AFPSPlayerState*>& Team1Order, const TArray<AFPSPlayerState*>& Team2Order)
{
	Team1PlayerOrder.Empty();
	Team2PlayerOrder.Empty();

	Team1PlayerOrder = Team1Order;
	Team2PlayerOrder = Team2Order;
	//Team1PlayerObjects = *Team1PlayerOrder;
	//Team2PlayerObjects = *Team2PlayerOrder;
	UE_LOG(LogClass, Log, TEXT("Team1PlayerOrderNUM: %d"), Team1PlayerOrder.Num());
	UE_LOG(LogClass, Log, TEXT("Team2PlayerOrderNUM: %d"), Team2PlayerOrder.Num());
	if (Team1PlayerOrder.IsValidIndex(0))
	{
		if (AFPSPlayerState* indexcontroller = Cast<AFPSPlayerState>(Team1PlayerOrder[0]))
		{
			//UE_LOG(LogClass, Log, TEXT("HI THERE TREY"));
			/*
			if (indexcontroller->GetPawn() != NULL) {
			//UE_LOG(LogClass, Log, TEXT("Team1FirstCharacter: %s"), *indexcontroller->GetPawn()->GetName());
			}
			UE_LOG(LogClass, Log, TEXT("Team1FirstPlayer: %s"), *FString::FromInt(UGameplayStatics::GetPlayerControllerID(indexcontroller)));
			*/
		}
	}

	//testcontroller = &controller;
	if (AFPSPlayerController* pc = Cast<AFPSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		pc->UpdateScoreBoardUI();
	}

}

bool AFPSGameState::ClientUpdateTeam1PlayerStates_Validate(AFPSPlayerState* PlayerState)
{
	return true;
}
void AFPSGameState::ClientUpdateTeam1PlayerStates_Implementation(AFPSPlayerState* PlayerState)
{
	Team1PlayerStates.Add(PlayerState);
}
bool AFPSGameState::ClientUpdateTeam2PlayerStates_Validate(AFPSPlayerState* PlayerState)
{
	return true;
}
void AFPSGameState::ClientUpdateTeam2PlayerStates_Implementation(AFPSPlayerState* PlayerState)
{
	Team2PlayerStates.Add(PlayerState);
}

bool AFPSGameState::ClientUpdateTeam1Controllers_Validate(AFPSPlayerController* FPSController)
{
	return true;
}
void AFPSGameState::ClientUpdateTeam1Controllers_Implementation(AFPSPlayerController* FPSController)
{
	Team1PlayerControllers.Add(FPSController);
	//Team1PlayerControllers = FPSControllers;
	UE_LOG(LogClass, Log, TEXT("Team 1 has %d controllers"), Team1PlayerControllers.Num());
}




void AFPSGameState::SetNumberOfPlayers_Implementation(int32 newNumberOfPlayers)
{
	NumberOfPlayers = newNumberOfPlayers;
}
bool AFPSGameState::ClientUpdateTeam2Controllers_Validate(AFPSPlayerController* FPSController)
{
	return true;
}
void AFPSGameState::ClientUpdateTeam2Controllers_Implementation(AFPSPlayerController* FPSController)
{
	Team2PlayerControllers.Add(FPSController);
	UE_LOG(LogClass, Log, TEXT("Team 2 has %d controllers"), Team1PlayerControllers.Num());
}

void AFPSGameState::ClientUpdateWinningTeam_Implementation(int32 team, FName WinningText)
{
	WinningTeam = team;
	WinningMessage = WinningText.ToString();
}

void AFPSGameState::Update()
{
	UWorld* World = GetWorld();
	check(World);







	if (GetCurrentState() == EGamePlayState::EPlaying)
	{
		/*
		for (FConstControllerIterator controller = World->GetControllerIterator(); controller; ++controller) {
		if (AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*controller)) {
		if (PlayerController->GetPlayerTeam() == 1)
		{
		Team1PlayerControllers.AddUnique(PlayerController);
		}
		if (PlayerController->GetPlayerTeam() == 2)
		{
		Team2PlayerControllers.AddUnique(PlayerController);
		}
		}
		}
		*/
		//UE_LOG(LogClass, Log, TEXT("KILLED"));
		for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It) {
			if (APlayerController* PlayerController = Cast<APlayerController>(*It)) {
				if (AFPSCharacter* Player = Cast<AFPSCharacter>(PlayerController->GetPawn())) {
					if (Player->GetCurrentHealth() <= 0.0f) {
						if (Player->Shooter == NULL) {
							Player->Shooter = Player;
						}
						//UE_LOG(LogClass, Log, TEXT("KILLED"));

						if (AFPSPlayerController* FPSController = Cast<AFPSPlayerController>(PlayerController))
						{
							FPSController->MyPlayerState->SetDeathsMultiCast(1);
						}
						if (AFPSPlayerController* ShooterController = Cast<AFPSPlayerController>(Player->Shooter->Controller))
						{
							if (ShooterController->MyPlayerState)
							{
								ShooterController->MyPlayerState->SetKillsMultiCast(1);
								ShooterController->MyPlayerState->SetScoreMultiCast(10);
							}

						}
						LastKiller = Player->Shooter->GetName();
						LastKilled = Player->GetName();

						KillFeedMessage = TEXT("" + LastKiller + " killed " + LastKilled);//(TEXT("%s killed %f"), LastKiller, LastKilled);

						ClientUpdateKillFeedMessage(FName(*KillFeedMessage));

						//SetNewKillFeedMessage(KillFeedMessage);

						if (AFPSPlayerController* FPSController = Cast<AFPSPlayerController>(Player->Shooter->Controller))
						{
							//if (AFPSPlayerState* playerstate = Cast<AFPSPlayerState>(FPSController->PlayerState))
							//{
							if (FPSController->GetPlayerTeam() != 0)
							{
								if (AFPSProjectGameModeBase* GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
								{
									if (FPSController->GetPlayerTeam() == 1)
									{
										ClientUpdateScore(1, GameMode->PointsPerKill);
										UpdateScoreBlueprintEvent();

									}
									if (FPSController->GetPlayerTeam() == 2)
									{
										ClientUpdateScore(2, GameMode->PointsPerKill);
										UpdateScoreBlueprintEvent();
										UE_LOG(LogClass, Log, TEXT("Team2 Scored"));

									}
								}

							}
							//}


						}


						////////////////////////////////////////////////////////////////////////////////////////////////


						TArray<AFPSPlayerState*> CorrectTeam1PlayerOrder;
						TArray<AFPSPlayerState*> CorrectTeam2PlayerOrder;
						if (Team1PlayerControllers.Num() + Team2PlayerControllers.Num() == NumberOfPlayers)
						{
							//Team2CorrectOrder.Empty();
							//UE_LOG(LogClass, Log, TEXT("Team2Controllers: %d"), Team2PlayerControllers.Num());
							int32 Team2BestScore = -1;

							for (int32 i = 0; i < Team2PlayerControllers.Num(); ++i)
							{

								if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(Team2PlayerControllers[i]->PlayerState))
								{
									if (ps->GetScore() > Team2BestScore)
									{
										CorrectTeam2PlayerOrder.Insert(ps, 0);
										Team2BestScore = ps->GetScore();
									}
									else if (CorrectTeam2PlayerOrder.Num() > 1)
									{
										for (int32 something = 0; something < CorrectTeam2PlayerOrder.Num(); ++something)
										{
											if (CorrectTeam2PlayerOrder.Contains(ps) == false)
											{
												if (AFPSPlayerState* OtherPS = Cast<AFPSPlayerState>(CorrectTeam2PlayerOrder[something]))
												{
													if (ps->GetScore() > OtherPS->GetScore())
													{
														CorrectTeam2PlayerOrder.Insert(ps, something);
													}
												}
												else
												{
													CorrectTeam2PlayerOrder.Emplace(ps);
												}
											}
										}
										if (CorrectTeam2PlayerOrder.Contains(ps) == false)
										{
											CorrectTeam2PlayerOrder.Emplace(ps);
										}
									}
									else
									{
										if (CorrectTeam2PlayerOrder.Contains(ps) == false)
										{
											CorrectTeam2PlayerOrder.Emplace(ps);

										}
									}

									//CorrectTeam2PlayerOrder.Emplace(ps);
									/*
									if (pc->GetScore() > BestScore)
									{
									//UE_LOG(LogClass, Log, TEXT("Team 2: Beat Best Score "));
									CorrectOrder.Insert(pc, 0);
									BestScore = pc->GetScore();
									}

									else
									{
									if (CorrectOrder.Num() > 1)
									{
									UE_LOG(LogClass, Log, TEXT("Team 1 Correct Order Greater than 1"));
									for (int32 index = 0; index < CorrectOrder.Num(); ++index)
									{
									if (AFPSPlayerController* pc2 = Cast<AFPSPlayerController>(CorrectOrder[index]))
									{
									if (pc->GetScore() > pc2->GetScore())
									{
									UE_LOG(LogClass, Log, TEXT("ScoreHigher "));
									CorrectOrder.Insert(pc, index);
									}



									}

									}
									if (CorrectOrder.Contains(pc))
									{

									}
									else
									{
									UE_LOG(LogClass, Log, TEXT("Added"));
									CorrectOrder.Emplace(pc);
									}
									}
									else
									{

									CorrectOrder.Emplace(pc);
									}



									}
									*/


								}


							}
							//UE_LOG(LogClass, Log, TEXT("i: %d"), i);
							//UE_LOG(LogClass, Log, TEXT("CorrectOrderTeam2: %d"), Team2CorrectOrder.Num());
							/*
							if (CorrectOrder.Num() > 1) {

							UE_LOG(LogClass, Log, TEXT("%s , %s, %s "), *FString::FromInt(UGameplayStatics::trtrollerID(CorrectOrder[0])), *FString::FromInt(UGameplayStatics::GetPlayerControllerID(CorrectOrder[1])), *FString::FromInt(UGameplayStatics::GetPlayerControllerID(CorrectOrder[2])));
							}
							*/




							//Team2CorrectOrder.Empty();
							//UE_LOG(LogClass, Log, TEXT("Team2Controllers: %d"), Team2PlayerControllers.Num());
							int32 Team1BestScore = -1;
							UE_LOG(LogClass, Log, TEXT("Team1PlayerControllersNUM: %d"), Team1PlayerControllers.Num());
							UE_LOG(LogClass, Log, TEXT("Team1PlayerStatesNUM: %d"), Team1PlayerStates.Num());
							for (int32 spot = 0; spot < Team1PlayerControllers.Num(); ++spot)
							{

								if (AFPSPlayerState* ps2 = Cast<AFPSPlayerState>(Team1PlayerControllers[spot]->PlayerState))
								{
									if (ps2->GetScore() > Team1BestScore)
									{
										UE_LOG(LogClass, Log, TEXT("Team1TopPlayer"));
										CorrectTeam1PlayerOrder.Insert(ps2, 0);
										Team1BestScore = ps2->GetScore();
									}
									else if (CorrectTeam1PlayerOrder.Num() > 1)
									{
										for (int32 something2 = 0; something2 < CorrectTeam1PlayerOrder.Num(); ++something2)
										{
											if (CorrectTeam1PlayerOrder.Contains(ps2) == false)
											{
												if (AFPSPlayerState* OtherPS2 = Cast<AFPSPlayerState>(CorrectTeam1PlayerOrder[something2]))
												{
													if (ps2->GetScore() > OtherPS2->GetScore())
													{
														UE_LOG(LogClass, Log, TEXT("Team1FoundWorsePlayer"));
														CorrectTeam1PlayerOrder.Insert(ps2, something2);
													}
												}
												else
												{
													CorrectTeam1PlayerOrder.Emplace(ps2);
												}
											}
										}
										if (CorrectTeam1PlayerOrder.Contains(ps2) == false)
										{
											CorrectTeam1PlayerOrder.Emplace(ps2);
										}

										//UE_LOG(LogClass, Log, TEXT("CorrectTeam1PlayerOrderNUM is > 1"));
									}
									else
									{
										if (CorrectTeam1PlayerOrder.Contains(ps2) == false)
										{
											CorrectTeam1PlayerOrder.Emplace(ps2);
											UE_LOG(LogClass, Log, TEXT("added to end"));

										}
									}
									/*
									if (pc->GetScore() > BestScore)
									{
									//UE_LOG(LogClass, Log, TEXT("Team 2: Beat Best Score "));
									CorrectOrder.Insert(pc, 0);
									BestScore = pc->GetScore();
									}

									else
									{
									if (CorrectOrder.Num() > 1)
									{
									UE_LOG(LogClass, Log, TEXT("Team 1 Correct Order Greater than 1"));
									for (int32 index = 0; index < CorrectOrder.Num(); ++index)
									{
									if (AFPSPlayerController* pc2 = Cast<AFPSPlayerController>(CorrectOrder[index]))
									{
									if (pc->GetScore() > pc2->GetScore())
									{
									UE_LOG(LogClass, Log, TEXT("ScoreHigher "));
									CorrectOrder.Insert(pc, index);
									}



									}

									}
									if (CorrectOrder.Contains(pc))
									{

									}
									else
									{
									UE_LOG(LogClass, Log, TEXT("Added"));
									CorrectOrder.Emplace(pc);
									}
									}
									else
									{

									CorrectOrder.Emplace(pc);
									}



									}
									*/


								}
								else
								{

								}


							}
							UE_LOG(LogClass, Log, TEXT("CorrectTeam1PlayerOrderNUM: %d"), CorrectTeam1PlayerOrder.Num());
							if (CorrectTeam2PlayerOrder.Num() == Team2PlayerControllers.Num())
							{
								Team2PlayerOrder = CorrectTeam2PlayerOrder;
							}
							if (CorrectTeam1PlayerOrder.Num() == Team1PlayerControllers.Num())
							{
								UE_LOG(LogClass, Log, TEXT("AYYYYYY"));
								Team1PlayerOrder = CorrectTeam1PlayerOrder;

								//testplayerstate = Team1PlayerOrder[0]->PlayerState;
								/*
								for (int32 i = 0; i < Team1PlayerOrder.Num(); ++i)
								{
								ClientsTeam1Controllers(Team1PlayerOrder[i]);
								UE_LOG(LogClass, Log, TEXT("CALLED CLIENT THING"));
								}
								for (FConstControllerIterator controller = World->GetControllerIterator(); controller; ++controller) {
								if (AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*controller)) {
								PlayerController->UpdateScoreBoardUI();
								}
								}
								*/
							}
							else
							{

								UE_LOG(LogClass, Log, TEXT("NOOOOO"));
							}
							//UE_LOG(LogClass, Log, TEXT("i: %d"), i);
							//UE_LOG(LogClass, Log, TEXT("CorrectOrderTeam2: %d"), Team2CorrectOrder.Num());
							/*
							if (CorrectOrder.Num() > 1) {

							UE_LOG(LogClass, Log, TEXT("%s , %s, %s "), *FString::FromInt(UGameplayStatics::GetPlayerControllerID(CorrectOrder[0])), *FString::FromInt(UGameplayStatics::GetPlayerControllerID(CorrectOrder[1])), *FString::FromInt(UGameplayStatics::GetPlayerControllerID(CorrectOrder[2])));
							}
							*/



							UpdateTeamOrder(Team1PlayerOrder, Team2PlayerOrder);



						}
						else
						{
							UE_LOG(LogClass, Log, TEXT("NOTSIXPLAYERSYET "));
						}

















						CallHUDUpdate();
						Player->OnPlayerDeath();


						//TheHUD->AddMessageEvent();










					}

				}
			}


			if (AFPSPlayerController* FPSController = Cast<AFPSPlayerController>(*It))
			{

				if (AFPSProjectGameModeBase* GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
				{
					if (FPSController->GetPlayerTeam() == 1)
					{
						float ScorePercent = ((double)Team1Score / GameMode->ScoreToWin);

						FPSController->UpdateMyTeamStats(Team1Score, ScorePercent);

					}
					if (FPSController->GetPlayerTeam() == 2)
					{
						//UE_LOG(LogClass, Log, TEXT(""));
						float ScorePercent = ((double)Team2Score / GameMode->ScoreToWin);

						FPSController->UpdateMyTeamStats(Team2Score, ScorePercent);

					}

				}
				AFPSPlayerController* MyRival = NULL;
				int32 RivalDifference = 100000;
				for (FConstControllerIterator OtherControllers = World->GetControllerIterator(); OtherControllers; ++OtherControllers) {
					if (AFPSPlayerController* OtherController = Cast<AFPSPlayerController>(*OtherControllers))
					{
						if (OtherController != FPSController)
						{
							if (OtherController->GetPlayerTeam() != FPSController->GetPlayerTeam())
							{


								if (OtherController->GetPlayerTeam() != 0)
								{
									if (OtherController->MyPlayerState) {
										if (FGenericPlatformMath::Abs(OtherController->MyTeamScore - FPSController->MyTeamScore) < RivalDifference)
										{
											MyRival = OtherController;
											RivalDifference = (FGenericPlatformMath::Abs(OtherController->MyTeamScore - FPSController->MyTeamScore));
										}
									}
								}
							}
						}
					}
				}
				if (MyRival != NULL)
				{
					if (AFPSProjectGameModeBase* GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode())) {

						if (MyRival->GetPlayerTeam() == 1)
						{
							float RivalScorePercent = ((double)Team1Score / GameMode->ScoreToWin);

							FPSController->UpdateRivalStats(MyRival->GetPlayerTeam(), MyRival->MyTeamScore, RivalScorePercent);
						}
						if (MyRival->GetPlayerTeam() == 2)
						{
							float RivalScorePercent = ((double)Team2Score / GameMode->ScoreToWin);

							FPSController->UpdateRivalStats(MyRival->GetPlayerTeam(), MyRival->MyTeamScore, RivalScorePercent);
						}
					}
				}
			}

		}
	}
	if (AFPSProjectGameModeBase* GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		if (Team1Score >= GameMode->ScoreToWin)
		{
			//UE_LOG(LogClass, Log, TEXT("GAME OVER"));
			WinningTeam = 1;
			WinningMessage = TEXT("Team 1 Won");
			ClientUpdateWinningTeam(WinningTeam, FName(*WinningMessage));
			CallHUDGameOver();
			for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It) {
				if (AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*It)) {
					if (AFPSCharacter* Player = Cast<AFPSCharacter>(PlayerController->GetPawn())) {
						Player->SetCurrentState(EPlayerState::EPlayerWaiting);
					}

				}

			}
			SetCurrentState(EGamePlayState::EGameOver);

		}
		if (Team2Score >= GameMode->ScoreToWin)
		{
			//UE_LOG(LogClass, Log, TEXT("GAME OVER"));
			WinningTeam = 2;
			WinningMessage = TEXT("Team 2 Won");
			ClientUpdateWinningTeam(WinningTeam, FName(*WinningMessage));
			CallHUDGameOver();
			for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It) {
				if (AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*It)) {
					if (AFPSCharacter* Player = Cast<AFPSCharacter>(PlayerController->GetPawn())) {
						Player->SetCurrentState(EPlayerState::EPlayerWaiting);
					}

				}

			}
			SetCurrentState(EGamePlayState::EGameOver);

		}
	}

}


int32 AFPSGameState::GetTeam1Score()
{
	return Team1Score;
}
int32 AFPSGameState::GetTeam2Score()
{
	return Team2Score;
}



void AFPSGameState::SetNewKillFeedMessage(FString Message)
{

	KillFeedMessage = TEXT("Something");

	/*
	if (APlayerHUD* HUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD())) {
	HUD->AddMessageEvent();
	}
	*/
}

void AFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPSGameState, KillFeedMessage);
	DOREPLIFETIME(AFPSGameState, testplayerstate);
	DOREPLIFETIME(AFPSGameState, CurrentState);
	DOREPLIFETIME(AFPSGameState, WinningPlayerName);
	DOREPLIFETIME(AFPSGameState, Team2PlayerStates);
	DOREPLIFETIME(AFPSGameState, Team1PlayerStates);
	DOREPLIFETIME(AFPSGameState, Team2PlayerControllers);
	DOREPLIFETIME(AFPSGameState, Team1PlayerControllers);
	DOREPLIFETIME(AFPSGameState, Team2PlayerOrder);
	DOREPLIFETIME(AFPSGameState, Team1PlayerOrder);

	DOREPLIFETIME(AFPSGameState, Team2PlayerObjects);
	DOREPLIFETIME(AFPSGameState, Team1PlayerObjects);

	DOREPLIFETIME(AFPSGameState, Team2CorrectOrder);
	DOREPLIFETIME(AFPSGameState, Team1CorrectOrder);

}

EGamePlayState AFPSGameState::GetCurrentState() const
{
	return CurrentState;
}

void AFPSGameState::SetCurrentState(EGamePlayState NewState)
{
	if (Role == ROLE_Authority) {
		CurrentState = NewState;
		if (CurrentState == EGamePlayState::EPlaying)
		{
			/*
			for (FConstControllerIterator controller = GetWorld()->GetControllerIterator(); controller; ++controller) {
			if (AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*controller)) {
			if (PlayerController->GetPlayerTeam() == 1)
			{
			Team1PlayerControllers.AddUnique(PlayerController);
			}
			if (PlayerController->GetPlayerTeam() == 2)
			{
			Team2PlayerControllers.AddUnique(PlayerController);
			}
			}
			}
			*/
			GetWorldTimerManager().SetTimer(UpdateTimer, this, &AFPSGameState::Update, UpdateDelay, true);
		}
		if (CurrentState == EGamePlayState::EGameOver)
		{
			//GetWorldTimerManager().ClearTimer(UpdateTimer);
		}
	}
}

void AFPSGameState::OnRep_CurrentState()
{

}

bool AFPSGameState::UpdateTeamScore_Validate(int32 Team)
{
	return true;
}

void AFPSGameState::UpdateTeamScore_Implementation(int32 Team)
{
	/*
	UE_LOG(LogClass, Log, TEXT("Reached Function call"));
	if (Role == ROLE_Authority)
	{
	UE_LOG(LogClass, Log, TEXT("SERVER"));
	if (AFPSProjectGameModeBase* GameMode = Cast<AFPSProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
	UE_LOG(LogClass, Log, TEXT("CASTED"));
	if (Team == 1)
	{
	int32 NewScore = Team1Score + GameMode->PointsPerKill;
	ClientUpdateScore(1, NewScore);
	Team1Score = NewScore;
	UpdateScoreBlueprintEvent();
	UE_LOG(LogClass, Log, TEXT("UPDATESCORE"));
	}
	}

	}
	*/
}




