// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSProjectGameModeBase.h"
#include "FPSCharacter.h"
#include "PlayerHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/GameFramework/GameMode.h"
#include "FPSPlayerState.h"


#include "Engine.h"

AFPSProjectGameModeBase::AFPSProjectGameModeBase()
{


	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_FPSCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set the type of HUD used in the game
	static ConstructorHelpers::FClassFinder<AHUD> PlayerHUDClass(TEXT("/Game/Blueprints/BP_PlayerHUD"));
	if (PlayerHUDClass.Class != NULL) {
		HUDClass = PlayerHUDClass.Class;
	}


	Team1Players = 0;
	Team2Players = 0;
	Team3Players = 0;
	Team4Players = 0;
	Team5Players = 0;
	SafeSpawnDistance = 1500;
	HasLoggedIn = false;
	NumTeamA = 0;
	NumTeamB = 0;
	SpawnDelay = 2;
	PointsPerKill = 1;

	AddKillFeedEntry = false;
	//set the type of gamestate used in the game
	GameStateClass = AFPSGameState::StaticClass();

	PlayerControllerClass = AFPSPlayerController::StaticClass();

	PlayerStateClass = AFPSPlayerState::StaticClass();
	NumberOfTeams = 2;
	ScoreToWin = 6;
}

void AFPSProjectGameModeBase::RespawnPlayer(APlayerController* NewPlayer)
{
	UE_LOG(LogClass, Log, TEXT("Respawning"));
	AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(NewPlayer);
	TArray<AFPSPlayerStart*> PreferredStarts;
	bool checkforsafety = true;
	for (int32 i = 0; i < 5; ++i) {
		for (TActorIterator<AFPSPlayerStart> PlayerStart(GetWorld()); PlayerStart; ++PlayerStart)
		{
			if (PlayerController)
			{
				if (PlayerStart->Tags.Contains("RespawnPoint"))
				{
					for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
					{
						AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*Iterator);
						if (PlayerController)
						{
							if (PlayerController != NewPlayer) {
								if (AFPSPlayerController* NewFPSPlayer = Cast<AFPSPlayerController>(NewPlayer))
								{


									if (PlayerController->GetPlayerTeam() != NewFPSPlayer->GetPlayerTeam())
									{
										AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(PlayerController->GetPawn());
										if (PlayerCharacter) {
											UCameraComponent* PlayerCamera = Cast<UCameraComponent>(PlayerCharacter->FPSCameraComponent);
											if (PlayerCamera)
											{
												FVector CameraLocation = PlayerCamera->GetComponentLocation();
												FVector SpawnPointLocation = PlayerStart->GetActorLocation();
												FVector DirectionBetween = (SpawnPointLocation - CameraLocation).GetSafeNormal();
												FVector CameraDirection = PlayerCamera->GetForwardVector().GetSafeNormal();
												float dotvalue = FGenericPlatformMath::Abs(FVector::DotProduct(DirectionBetween, CameraDirection));

												if (dotvalue > .68f) {


													FHitResult hit;

													if (GetWorld()->LineTraceSingleByChannel(hit, CameraLocation, CameraLocation + (DirectionBetween * 10000), ECollisionChannel::ECC_Camera))
													{


														if (AFPSPlayerStart* spawnpoint = Cast<AFPSPlayerStart>(hit.GetActor())) {

														}
														else {


															PreferredStarts.AddUnique(*PlayerStart);
														}
													}

													if (i >= 3) {
														UE_LOG(LogClass, Log, TEXT("Respawning anyway"));
														PreferredStarts.AddUnique(*PlayerStart);
													}

												}
												else
												{
													if (FVector::Dist(CameraLocation, SpawnPointLocation) > SafeSpawnDistance || i >= 3) {
														PreferredStarts.AddUnique(*PlayerStart);
													}

												}

											}
										}
									}
								}
							}
						}

					}
				}
			}
		}
		if (PreferredStarts.Num() == 0) {

		}
		else
		{
			break;
		}
	}
	if (PreferredStarts.Num() > 1) {
		AFPSPlayerStart* BestStart = PreferredStarts[FMath::RandRange(0, PreferredStarts.Num() - 1)];
		float SpawnDistance = 0.0f;
		for (int32 i = 0; i < PreferredStarts.Num(); ++i) {
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{

				AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*Iterator);
				if (PlayerController)
				{
					if (PlayerController != NewPlayer) {
						if (AFPSPlayerController* NewFPSPlayer = Cast<AFPSPlayerController>(NewPlayer))
						{
							if (PlayerController->GetPlayerTeam() != NewFPSPlayer->GetPlayerTeam())
							{


								AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(PlayerController->GetPawn());
								if (PlayerCharacter) {
									UCameraComponent* PlayerCamera = Cast<UCameraComponent>(PlayerCharacter->FPSCameraComponent);
									if (PlayerCamera)
									{
										FVector CameraLocation = PlayerCamera->GetComponentLocation();
										FVector SpawnPointLocation = PreferredStarts[i]->GetActorLocation();
										if (FVector::Dist(CameraLocation, SpawnPointLocation) > SpawnDistance)
										{
											BestStart = PreferredStarts[i];
											SpawnDistance = FVector::Dist(CameraLocation, SpawnPointLocation);
										}
									}
								}
							}
						}
					}
				}
			}

		}
		int32 PlayerStartIndex = FMath::RandRange(0, PreferredStarts.Num() - 1);
		NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, BestStart));

		RestartPlayer(NewPlayer);
		AFPSCharacter* Character = Cast<AFPSCharacter>(NewPlayer->GetPawn());
		if (Character)
		{
			Character->TriggerAddUI();

		}
		if (PlayerController)
		{

		}
	}
	else {
		int32 PlayerStartIndex = 0;
		NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, PreferredStarts[PlayerStartIndex]));

		RestartPlayer(NewPlayer);
		AFPSCharacter* Character = Cast<AFPSCharacter>(NewPlayer->GetPawn());
		if (Character)
		{
			Character->TriggerAddUI();

		}
		if (PlayerController)
		{

		}
	}

}



void AFPSProjectGameModeBase::StartNewPlayerClient(APlayerController* NewPlayer)
{
	
	if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
	{
		MyGameState->SetNumberOfPlayers(GetNumPlayers());
	}
	NewPlayer->GetPawn()->Destroy();
	AFPSPlayerController* TestPlayerController = Cast<AFPSPlayerController>(NewPlayer);
	TArray<AFPSPlayerStart*> PreferredStarts;
	bool BlockCheck = true;
	for (int32 i = 0; i < 2; ++i) {

		for (TActorIterator<AFPSPlayerStart> PlayerStart(GetWorld()); PlayerStart; ++PlayerStart)
		{

			if (TestPlayerController)
			{
				if (TestPlayerController->GetPlayerTeam() == 1 && PlayerStart->Tags.Contains("Team1")) //&& PlayerStart->PlayerStartTag != FName(TEXT("Blocked")))
				{
					if (PlayerStart->PlayerStartTag == FName(TEXT("Blocked"))) {
						if (BlockCheck == false) {

							PreferredStarts.AddUnique(*PlayerStart);
						}
						else {

						}

					}
					else
					{

						PreferredStarts.AddUnique(*PlayerStart);
					}
					// Player should spawn on CT.


				}
				else if (TestPlayerController->GetPlayerTeam() == 2 && PlayerStart->Tags.Contains("Team2")) //&& PlayerStart->PlayerStartTag != FName(TEXT("Blocked")))
				{
					if (PlayerStart->PlayerStartTag == FName(TEXT("Blocked"))) {
						if (BlockCheck == false) {

							PreferredStarts.AddUnique(*PlayerStart);
						}
						else {

						}

					}
					else {

						PreferredStarts.AddUnique(*PlayerStart);
					}
					// Player should spawn on Suspects


				}

			}

		}
		if (PreferredStarts.Num() == 0) {
			BlockCheck = false;
		}
		else {
			break;
		}

	}
	if (TestPlayerController && TestPlayerController->GetPlayerTeam() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Player is a spectator");
		return;
	}
	else
	{
		if (TestPlayerController) {
			if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(TestPlayerController->PlayerState))
			{
				if (ps->TeamNumber == 1)
				{
					if (ABaseTeam* Team1 = Cast<ABaseTeam>(Teams[0])) {
						Team1->TeamPlayerStates.Add(ps);
						ps->SetTeam(Team1);
					}
				}
				if (ps->TeamNumber == 2)
				{
					if (ABaseTeam* Team2 = Cast<ABaseTeam>(Teams[1])) {
						Team2->TeamPlayerStates.Add(ps);
						ps->SetTeam(Team2);
					}
				}
			}
		}


		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Player has selected a Team");
		int32 PlayerStartIndex = FMath::RandRange(0, PreferredStarts.Num() - 1);
		NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, PreferredStarts[PlayerStartIndex]));
		PreferredStarts[PlayerStartIndex]->PlayerStartTag = FName(TEXT("Blocked"));

		RestartPlayer(NewPlayer);
		AFPSCharacter* Character = Cast<AFPSCharacter>(NewPlayer->GetPawn());
		if (Character)
		{
			Character->TriggerAddUI();

		}

		if (TestPlayerController)
		{
			if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
			{
				MyGameState->ClientUpdateNumberOfTeams(NumberOfTeams);
				//TestPlayerController->TriggerAddAliveUI();
			}
			//TestPlayerController->TriggerAddAliveUI();
		}

	}
	//UE_LOG(LogClass, Log, TEXT("NumberofPlayers: %d"), GetNumPlayers());
	if (Team1PlayerControllers.Num() + Team2PlayerControllers.Num() == GetNumPlayers() - 1)
	{
		UE_LOG(LogClass, Log, TEXT("Totalcontrollers: %d"), Team1PlayerControllers.Num() + Team2PlayerControllers.Num());
		UE_LOG(LogClass, Log, TEXT("Team1: %d"), Team1PlayerControllers.Num());
		UE_LOG(LogClass, Log, TEXT("Team2: %d"), Team2PlayerControllers.Num());
	}
	
	if (Team1PlayerControllers.Num() + Team2PlayerControllers.Num() == GetNumPlayers())
	{
		/*
		UE_LOG(LogClass, Log, TEXT("PUTTERSLOVE"));
		if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
		{
			if (MyGameState->Team1PlayerControllers.Num() == Team1PlayerControllers.Num() && MyGameState->Team2PlayerControllers.Num() == Team2PlayerControllers.Num())
			{
				for (int32 i = 0; i < MyGameState->Team1PlayerStates.Num(); ++i)
				{
					if (AFPSPlayerState* PlayerState = Cast<AFPSPlayerState>(MyGameState->Team1PlayerStates[i]))
					{

						FString newName = "Team1Player[" + FString::FromInt(i) + "]";
						PlayerState->SetUserNameMultiCast(FName(*newName));
						UE_LOG(LogClass, Log, TEXT("SetTeam1PlayerName"));
					}
					else
					{
						UE_LOG(LogClass, Log, TEXT("FailedtoCastTeam1"));
					}

				}
				UE_LOG(LogClass, Log, TEXT("there are %d team 2 player states"), MyGameState->Team1PlayerStates.Num());
				for (int32 i = 0; i < MyGameState->Team2PlayerStates.Num(); ++i)
				{
					if (AFPSPlayerState* PlayerState = Cast<AFPSPlayerState>(MyGameState->Team2PlayerStates[i]))
					{

						FString newName = "Team2Player[" + FString::FromInt(i) + "]";
						PlayerState->SetUserNameMultiCast(FName(*newName));
						UE_LOG(LogClass, Log, TEXT("SetTeam2PlayerName"));

					}
					else
					{
						UE_LOG(LogClass, Log, TEXT("FailedtoCastTeam2"));
					}

				}
				//MyGameState->Team1PlayerControllers = Team1PlayerControllers;
				//MyGameState->Team2PlayerControllers = Team2PlayerControllers;
				UE_LOG(LogClass, Log, TEXT("GameStateUpdated"));
				for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
				{
					AFPSPlayerController* PC = Cast<AFPSPlayerController>(*It);
					PC->AddScoreBoardUI();
					PC->TriggerAddAliveUI();
				}
			}
		}
		*/
	}
	

}


void AFPSProjectGameModeBase::StartNewPlayer(APlayerController* NewPlayer)
{


	AFPSPlayerController* TestPlayerController = Cast<AFPSPlayerController>(NewPlayer);

	TArray<AFPSPlayerStart*> PreferredStarts;
	if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
	{
		MyGameState->SetNumberOfPlayers(GetNumPlayers());
	}


	for (TActorIterator<AFPSPlayerStart> PlayerStart(GetWorld()); PlayerStart; ++PlayerStart)
	{


		if (TestPlayerController)
		{

			if (TestPlayerController->GetPlayerTeam() == 1 && PlayerStart->Tags.Contains("Team1"))
			{

				// Player should spawn on CT.
				PreferredStarts.Add(*PlayerStart);

			}
			else if (TestPlayerController->GetPlayerTeam() == 2 && PlayerStart->Tags.Contains("Team2"))
			{

				// Player should spawn on Suspects
				PreferredStarts.Add(*PlayerStart);

			}

		}
	}
	if (TestPlayerController && TestPlayerController->GetPlayerTeam() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Player is a spectator");
		return;
	}
	else
	{
		if (TestPlayerController->GetPlayerTeam() == 1) {

			Team1Players += 1;
			if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(TestPlayerController->PlayerState))
			{
				ps->TeamNumber = 1;
				if (ABaseTeam* Team1 = Cast<ABaseTeam>(Teams[0]))
				{
					//Team1->TeamNumber = 1;
					Team1->TeamPlayerStates.Add(ps);
					Team1->TeamColor = FColor::Blue;
					ps->SetTeam(Team1);
					UE_LOG(LogClass, Log, TEXT("added to team number 1 player states"));
				}
			}
			Team1PlayerControllers.AddUnique(TestPlayerController);
			if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
			{
				MyGameState->ClientUpdateTeam1Controllers(TestPlayerController);
				if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(TestPlayerController->PlayerState))
				{
					MyGameState->ClientUpdateTeam1PlayerStates(ps);
				}

			}
			
			UE_LOG(LogClass, Log, TEXT("Team1PlayerControllerslength: %d"), Team1PlayerControllers.Num());
			UE_LOG(LogClass, Log, TEXT("Team2PlayerControllerslength: %d"), Team2PlayerControllers.Num());

			{
				//MyGameState->ClientUpdateTeam1Controllers(TestPlayerController);
				//UE_LOG(LogClass, Log, TEXT("added server to team 1"));

			}
		}
		if (TestPlayerController->GetPlayerTeam() == 2) {
			Team2Players += 1;
			if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(TestPlayerController->PlayerState))
			{
				ps->TeamNumber = 2;
				if (ABaseTeam* Team2 = Cast<ABaseTeam>(Teams[1]))
				{
					//Team2->TeamNumber = 2;
					Team2->TeamPlayerStates.Add(ps);
					Team2->TeamColor = FColor::Red;
					ps->SetTeam(Team2);
					UE_LOG(LogClass, Log, TEXT("added to team number 2 player states"));
				}
			}
			Team2PlayerControllers.AddUnique(TestPlayerController);
			if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
			{
				MyGameState->ClientUpdateTeam2Controllers(TestPlayerController);
				if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(TestPlayerController->PlayerState))
				{
					MyGameState->ClientUpdateTeam2PlayerStates(ps);
				}
			}
			/*
			if (Team1PlayerControllers.Num() + Team2PlayerControllers.Num() == GetNumPlayers())
			{
				UE_LOG(LogClass, Log, TEXT("PUTTERSLOVE"));
				if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
				{
					if (MyGameState->Team1PlayerControllers.Num() == Team1PlayerControllers.Num() && MyGameState->Team2PlayerControllers.Num() == Team2PlayerControllers.Num())
					{
						for (int32 i = 0; i < MyGameState->Team1PlayerStates.Num(); ++i)
						{
							if (AFPSPlayerState* PlayerState = Cast<AFPSPlayerState>(MyGameState->Team1PlayerStates[i]))
							{

								FString newName = "Team1Player[" + FString::FromInt(i) + "]";
								PlayerState->SetUserNameMultiCast(FName(*newName));
								UE_LOG(LogClass, Log, TEXT("SetTeam1PlayerName"));
							}
							else
							{
								UE_LOG(LogClass, Log, TEXT("FailedtoCastTeam1"));
							}

						}
						UE_LOG(LogClass, Log, TEXT("there are %d team 2 player states"), MyGameState->Team1PlayerStates.Num());
						for (int32 i = 0; i < MyGameState->Team2PlayerStates.Num(); ++i)
						{
							if (AFPSPlayerState* PlayerState = Cast<AFPSPlayerState>(MyGameState->Team2PlayerStates[i]))
							{

								FString newName = "Team2Player[" + FString::FromInt(i) + "]";
								PlayerState->SetUserNameMultiCast(FName(*newName));
								UE_LOG(LogClass, Log, TEXT("SetTeam2PlayerName"));

							}
							else
							{
								UE_LOG(LogClass, Log, TEXT("FailedtoCastTeam2"));
							}

						}
						//MyGameState->Team1PlayerControllers = Team1PlayerControllers;
						//MyGameState->Team2PlayerControllers = Team2PlayerControllers;
						UE_LOG(LogClass, Log, TEXT("GameStateUpdated"));
						for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
						{
							AFPSPlayerController* PC = Cast<AFPSPlayerController>(*It);
							PC->AddScoreBoardUI();
							PC->TriggerAddAliveUI();
						}
					}
				}

			}
			*/
			UE_LOG(LogClass, Log, TEXT("Team1PlayerControllerslength: %d"), Team1PlayerControllers.Num());
			UE_LOG(LogClass, Log, TEXT("Team2PlayerControllerslength: %d"), Team2PlayerControllers.Num());

		}
		if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
		{


			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(*Iterator);
				if (PlayerController) {
					if (PlayerController != TestPlayerController) {
						if (Team1Players > Team2Players) {
							if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(PlayerController->PlayerState))
							{
								ps->TeamNumber = 2;
							}
							PlayerController->ServerSetPlayerTeamClient(2);

							Team2Players++;
							MyGameState->Team2Players = Team2Players;
							UE_LOG(LogClass, Log, TEXT("added client to team 2"));
							Team2PlayerControllers.AddUnique(PlayerController);
							if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
							{
								MyGameState->ClientUpdateTeam2Controllers(PlayerController);
								if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(PlayerController->PlayerState))
								{
									MyGameState->ClientUpdateTeam2PlayerStates(ps);
								}
							}
							if (Team1PlayerControllers.Num() + Team2PlayerControllers.Num() == GetNumPlayers())
							{
								UE_LOG(LogClass, Log, TEXT("Everyoneassigned"));
								if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
								{
									
									UE_LOG(LogClass, Log, TEXT("there are %d team 1 player states"),MyGameState->Team1PlayerStates.Num());
									for (int32 i = 0; i < MyGameState->Team1PlayerStates.Num(); ++i)
										{
										if (AFPSPlayerState* PlayerState = Cast<AFPSPlayerState>(MyGameState->Team1PlayerStates[i]))
										{

											FString newName = "Team1Player[" + FString::FromInt(i) + "]";
											PlayerState->SetUserNameMultiCast(FName(*newName));
											UE_LOG(LogClass, Log, TEXT("SetTeam1PlayerName"));
										}
										else
										{
											UE_LOG(LogClass, Log, TEXT("FailedtoCastTeam1"));
										}

										}
										UE_LOG(LogClass, Log, TEXT("there are %d team 2 player states"), MyGameState->Team1PlayerStates.Num());
										for (int32 i = 0; i < MyGameState->Team2PlayerStates.Num(); ++i)
										{
											if (AFPSPlayerState* PlayerState = Cast<AFPSPlayerState>(MyGameState->Team2PlayerStates[i]))
											{

												FString newName = "Team2Player[" + FString::FromInt(i) + "]";
												PlayerState->SetUserNameMultiCast(FName(*newName));
												UE_LOG(LogClass, Log, TEXT("SetTeam2PlayerName"));

											}
										else
										{
											UE_LOG(LogClass, Log, TEXT("FailedtoCastTeam2"));
										}

									}
									
									//MyGameState->Team1PlayerControllers = Team1PlayerControllers;
									//MyGameState->Team2PlayerControllers = Team2PlayerControllers;
									UE_LOG(LogClass, Log, TEXT("GameStateUpdated"));
									for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
									{

										AFPSPlayerController* PC = Cast<AFPSPlayerController>(*It);
										PC->AddScoreBoardUI();
										PC->TriggerAddAliveUI();
									}

								}
								else
								{
									UE_LOG(LogClass, Log, TEXT("no game state cast"));
								}
							}
							UE_LOG(LogClass, Log, TEXT("Team1PlayerControllerslength: %d"), Team1PlayerControllers.Num());
							UE_LOG(LogClass, Log, TEXT("Team2PlayerControllerslength: %d"), Team2PlayerControllers.Num());


						}
						else {

							Team1Players++;
							if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(PlayerController->PlayerState))
							{
								ps->TeamNumber = 1;
							}
							PlayerController->ServerSetPlayerTeamClient(1);
							MyGameState->Team1Players = Team1Players;
							UE_LOG(LogClass, Log, TEXT("added client to team 1"));
							Team1PlayerControllers.AddUnique(PlayerController);
							if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
							{
								MyGameState->ClientUpdateTeam1Controllers(PlayerController);
								if (AFPSPlayerState* ps = Cast<AFPSPlayerState>(PlayerController->PlayerState))
								{
									MyGameState->ClientUpdateTeam1PlayerStates(ps);
								}
							}
							
							if (Team1PlayerControllers.Num() + Team2PlayerControllers.Num() == GetNumPlayers())
							{
								UE_LOG(LogClass, Log, TEXT("PUTTERSLOVE"));
								if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
								{
									for (int32 i = 0; i < MyGameState->Team1PlayerStates.Num(); ++i)
									{
										if (AFPSPlayerState* PlayerState = Cast<AFPSPlayerState>(MyGameState->Team1PlayerStates[i]))
										{

											FString newName = "Team1Player[" + FString::FromInt(i) + "]";
											PlayerState->SetUserNameMultiCast(FName(*newName));
											UE_LOG(LogClass, Log, TEXT("SetTeam1PlayerName"));
										}
										else
										{
											UE_LOG(LogClass, Log, TEXT("FailedtoCastTeam1"));
										}

									}
									UE_LOG(LogClass, Log, TEXT("there are %d team 2 player states"), MyGameState->Team1PlayerStates.Num());
									for (int32 i = 0; i < MyGameState->Team2PlayerStates.Num(); ++i)
									{
										if (AFPSPlayerState* PlayerState = Cast<AFPSPlayerState>(MyGameState->Team2PlayerStates[i]))
										{

											FString newName = "Team2Player[" + FString::FromInt(i) + "]";
											PlayerState->SetUserNameMultiCast(FName(*newName));
											UE_LOG(LogClass, Log, TEXT("SetTeam2PlayerName"));

										}
										else
										{
											UE_LOG(LogClass, Log, TEXT("FailedtoCastTeam2"));
										}

									}
									//MyGameState->Team1PlayerControllers = Team1PlayerControllers;
									//MyGameState->Team2PlayerControllers = Team2PlayerControllers;
									UE_LOG(LogClass, Log, TEXT("GameStateUpdated"));
									for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
									{
										AFPSPlayerController* PC = Cast<AFPSPlayerController>(*It);
										PC->AddScoreBoardUI();
										PC->TriggerAddAliveUI();
									}
								}
							}
							
							UE_LOG(LogClass, Log, TEXT("Team1PlayerControllerslength: %d"), Team1PlayerControllers.Num());
							UE_LOG(LogClass, Log, TEXT("Team2PlayerControllerslength: %d"), Team2PlayerControllers.Num());

						}
					}

				}


			}
		}
		HandleNewState(EGamePlayState::EPlaying);
		NewPlayer->GetPawn()->Destroy();
		int32 StartSpotIndex = FMath::RandRange(0, PreferredStarts.Num() - 1);
		NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, PreferredStarts[StartSpotIndex]));
		for (TActorIterator<AFPSPlayerStart> ClearPlayerStart(GetWorld()); ClearPlayerStart; ++ClearPlayerStart) {
			ClearPlayerStart->PlayerStartTag = FName(TEXT("Open"));
		}

		PreferredStarts[StartSpotIndex]->PlayerStartTag = FName(TEXT("Blocked"));

		RestartPlayer(NewPlayer);
		AFPSCharacter* Character = Cast<AFPSCharacter>(NewPlayer->GetPawn());
		if (Character)
		{
			Character->TriggerAddUI();

		}
		if (TestPlayerController)
		{
			if (AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState))
			{
				MyGameState->ClientUpdateNumberOfTeams(NumberOfTeams);
				//TestPlayerController->TriggerAddAliveUI();
			}


		}
	}


	//TArray<AFPSPlayerController> PlayerControllers;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFPSPlayerController::StaticClass(), PlayerControllers);


}







void AFPSProjectGameModeBase::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

}


void AFPSProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	check(World);
	AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState);
	check(MyGameState);
	MyGameState->ClientUpdateNumberOfTeams(NumberOfTeams);

	//HandleNewState(EGamePlayState::EPlaying);

	for (int32 i = 1; i <= NumberOfTeams; ++i)
	{
		ABaseTeam* Team = GetWorld()->SpawnActor<ABaseTeam>(BaseTeamObj, FVector(0, 0, 20), FRotator(0, 0, 0));
		Team->TeamNumber = i;
		if (i == 1)
		{
			Team->TeamColor = FColor::Blue;
		}
		if (i == 2)
		{
			Team->TeamColor = FColor::Red;
		}
		Teams.Add(Team);
		if (Team)
		{
			UE_LOG(LogClass, Log, TEXT("Number of Teams: %d"), Teams.Num());
		}
	}

	MyGameState->SetNumberOfPlayers(GetNumPlayers());



}



void AFPSProjectGameModeBase::HandleNewState(EGamePlayState NewState)
{
	UWorld* World = GetWorld();
	check(World);
	AFPSGameState* MyGameState = Cast<AFPSGameState>(GameState);
	check(MyGameState);

	if (NewState != MyGameState->GetCurrentState())
	{
		//update the state, so clients know about the transition
		MyGameState->SetCurrentState(NewState);

		switch (NewState)
		{
		case EGamePlayState::EWaiting:

			break;
		case EGamePlayState::EPlaying:


			break;
		case EGamePlayState::EGameOver:


			break;
		default:
		case EGamePlayState::EUnknown:
			break;
		}
	}

}


