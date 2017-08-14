// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacter.h"
#include "AssaultRifle.h"
#include "FPSPlayerController.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFPSCharacter::AFPSCharacter()
{
	CurrentState = EPlayerState::EPlayerPlaying;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	//bReplicates = true;
	// Create a first person camera component.
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	// Attach the camera component to our capsule component.
	FPSCameraComponent->SetupAttachment(GetCapsuleComponent());

	// Position the camera slightly above the eyes.
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	// Allow the pawn to control camera rotation.
	FPSCameraComponent->bUsePawnControlRotation = true;
	DefaultFOV = FPSCameraComponent->FieldOfView;
	// Create a first person mesh component for the owning player.
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	// Only the owning player sees this mesh.
	FPSMesh->SetOnlyOwnerSee(true);
	// Attach the FPS mesh to the FPS camera.
	FPSMesh->SetupAttachment(FPSCameraComponent);
	// Disable some environmental shadowing to preserve the illusion of having a single mesh.
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	// The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(true);

	CollectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollectionBox"));
	CollectionBox->SetupAttachment(RootComponent);
	CollectionBox->bGenerateOverlapEvents = true;

	//set base weapon values
	IsFiring = false;
	HasWeapon = false;
	InitialHealth = 100.0f;
	CurrentHealth = InitialHealth;
	HealthPercentage = 1.0f;


}


void AFPSCharacter::TriggerUpdateUIBlueprint_Implementation() {

}

bool AFPSCharacter::TriggerUpdateUI_Validate() {
	return true;
}

void AFPSCharacter::TriggerUpdateUI_Implementation()
{

	TriggerUpdateUIBlueprint();

}

void AFPSCharacter::TriggerAddUIBlueprint_Implementation() {

}

bool AFPSCharacter::TriggerAddUI_Validate() {
	return true;
}

void AFPSCharacter::TriggerAddUI_Implementation()
{

	TriggerAddUIBlueprint();

}

void AFPSCharacter::TriggerDeathUIBlueprint_Implementation() {

}


bool AFPSCharacter::TriggerDeathUI_Validate() {
	return true;
}

void AFPSCharacter::TriggerDeathUI_Implementation()
{

	TriggerDeathUIBlueprint();

}

EPlayerState AFPSCharacter::GetCurrentState() const
{
	return CurrentState;
}
void AFPSCharacter::SetCurrentState(EPlayerState NewState)
{
	if (Role == ROLE_Authority) {
		CurrentState = NewState;
	}
}
void AFPSCharacter::OnPlayerDeath_Implementation()
{
	DropWeapon();
	SetCurrentState(EPlayerState::EPlayerDead);
	TriggerDeathUI();
	// disconnect controller from pawn
	DetachFromControllerPendingDestroy();

	if (GetMesh())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
	}
	SetActorEnableCollision(true);
	//ragdoll (init physics)
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
	GetMesh()->SetOwnerNoSee(false);
	//disable movement
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);
	//disable collisions on the capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (Iterator->Get()->AcknowledgedPawn == this) {
			APlayerController* PController = Iterator->Get();
			AFPSPlayerController* PC = Cast<AFPSPlayerController>(PController);
			if (PC)
			{

				PC->OnKilled();
			}
			else
			{
				UE_LOG(LogClass, Log, TEXT("cast failed"));
			}
		}
	}


}

float AFPSCharacter::GetCurrentHealth()
{
	return CurrentHealth;
}



void AFPSCharacter::ServerChangeHealthBy_Implementation(float delta)
{
	CurrentHealth += delta;
	HealthPercentage = CurrentHealth / InitialHealth;

}
float AFPSCharacter::GetInitialHealth()
{
	return InitialHealth;
}

bool  AFPSCharacter::ServerUpdateRotation_Validate(FRotator Rotation)
{

	return true;

}


void  AFPSCharacter::ServerUpdateRotation_Implementation(FRotator Rotation)
{

	FPSCameraComponent->SetWorldRotation(Rotation);

}


void AFPSCharacter::OnShoot()
{

	if (CurrentPrimary != NULL) {
		ServerOnShoot();
	}

}
void AFPSCharacter::OnStopShoot()
{
	ServerOnStopShoot();
}

bool AFPSCharacter::ServerOnZoom_Validate()
{
	return true;
}
void AFPSCharacter::ServerOnZoom_Implementation()
{
	if (CurrentPrimary != NULL)
	{



		SetIsZoomed(true);
		GetWorld()->GetTimerManager().SetTimer(ZoomTimer, this, &AFPSCharacter::Zoom, .005f, true);



	}
}
bool AFPSCharacter::ServerOnStopZoom_Validate()
{
	return true;
}
void AFPSCharacter::ServerOnStopZoom_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(ZoomTimer);
	SetIsZoomed(false);


	GetWorld()->GetTimerManager().SetTimer(ZoomTimer, this, &AFPSCharacter::Zoom, .005f, true);


}
bool AFPSCharacter::Zoom_Validate()
{
	return true;
}

void AFPSCharacter::Zoom_Implementation()
{
	//UE_LOG(LogClass, Log, TEXT("ZOOMING"))
	if (CurrentPrimary != NULL)
	{
		if (IsZoomed == true)
		{
			FPSCameraComponent->SetFieldOfView(FMath::Lerp(FPSCameraComponent->FieldOfView, CurrentPrimary->ZoomFOV, CurrentPrimary->ZoomRate));
		}
		else
		{
			if (FMath::IsNearlyEqual(FPSCameraComponent->FieldOfView, DefaultFOV, .01f))
			{
				GetWorld()->GetTimerManager().ClearTimer(ZoomTimer);
				UE_LOG(LogClass, Log, TEXT("FullyZoomedOut"))
			}
			else
			{
				FPSCameraComponent->SetFieldOfView(FMath::Lerp(FPSCameraComponent->FieldOfView, DefaultFOV, CurrentPrimary->ZoomRate));
			}
		}
	}
	else
	{
		if (FMath::IsNearlyEqual(FPSCameraComponent->FieldOfView, DefaultFOV, 0.1f))
		{
			GetWorld()->GetTimerManager().ClearTimer(ZoomTimer);
		}
		else
		{
			FPSCameraComponent->SetFieldOfView(FMath::Lerp(FPSCameraComponent->FieldOfView, DefaultFOV, CurrentPrimary->ZoomRate));
		}
	}

}

bool AFPSCharacter::ServerOnStopShoot_Validate()
{
	return true;
}
void AFPSCharacter::ServerOnStopShoot_Implementation()
{
	if (Role == ROLE_Authority) {
		IsFiring = false;
		UWorld* World = GetWorld();
		check(World);
		GetWorld()->GetTimerManager().ClearTimer(WeaponFireRateTimer);
	}
}

void AFPSCharacter::FireAgain()
{
	ServerOnShoot();
}
bool AFPSCharacter::ServerAddGunRecoil_Validate(float RecoilValue)
{
	return true;
}
void AFPSCharacter::ServerAddGunRecoil_Implementation(float RecoilValue)
{
	float TrueRecoilValue = RecoilValue / 8.0f;
	float RandomPitchValue = FMath::RandRange(TrueRecoilValue * 0.7f, TrueRecoilValue * 1.3f);
	AddControllerPitchInput(-RandomPitchValue);
	float RandomYawValue = FMath::RandRange(-TrueRecoilValue / 2.0f, +TrueRecoilValue / 2.0f);
	AddControllerYawInput(RandomYawValue);
}

bool AFPSCharacter::SetIsZoomed_Validate(bool zoomvalue)
{
	return true;
}
void AFPSCharacter::SetIsZoomed_Implementation(bool zoomvalue)
{
	IsZoomed = zoomvalue;
}
bool AFPSCharacter::ServerOnShoot_Validate()
{
	return true;
}
void AFPSCharacter::ServerOnShoot_Implementation()
{
	if (AFPSGameState* const GameState = Cast<AFPSGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->GetCurrentState() == EGamePlayState::EPlaying)
		{
			if (Role = ROLE_Authority) {
				UWorld* World = GetWorld();

				check(World);

				FHitResult hit;
				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(this);
				if (CurrentPrimary->AmmoLeftInMag > 0 && IsFiring == false && CurrentPrimary->CanFire == true) {
					if (IsZoomed)
					{
						ServerAddGunRecoil(CurrentPrimary->ZoomRecoilValue);
					}
					else
					{
						ServerAddGunRecoil(CurrentPrimary->RecoilValue);
					}
					UE_LOG(LogClass, Log, TEXT("Max Spread: %f"), CurrentPrimary->MaxSpread);
					UE_LOG(LogClass, Log, TEXT("Accuracy value: %f"), AccuracySpreadValue);
					if (CurrentPrimary->IsProjectile == false) {
						if (CurrentPrimary->IsAutomatic == true)
						{
							GetWorld()->GetTimerManager().SetTimer(WeaponFireRateTimer, this, &AFPSCharacter::FireAgain, CurrentPrimary->FireRate, false);
						}

						FVector BulletDestination;
						FVector TestBulletDestination = FVector(FPSCameraComponent->GetComponentLocation() + (FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range));
						/*
						if (World->LineTraceSingleByChannel(testhit,FPSCameraComponent->GetComponentLocation(), FPSCameraComponent->GetComponentLocation() + (FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range * AccuracyChange), ECollisionChannel::ECC_Visibility, QueryParams))
						{
						BulletDestination = testhit.Location;
						}
						else
						{
						BulletDestination = FPSCameraComponent->GetComponentLocation() + (FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range * AccuracyChange);
						}
						*/
						if (World->LineTraceSingleByChannel(hit, FPSCameraComponent->GetComponentLocation(), FPSCameraComponent->GetComponentLocation() + (FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range), ECollisionChannel::ECC_Visibility, QueryParams)) {
							//DrawDebugLine(World, CurrentPrimary->MuzzleLocation->GetComponentLocation(), hit.Location, FColor::Red, false, 3.0f);
							float distancefactor = hit.Distance / 1000.0f;
							float RandYSpread = FMath::RandRange(-AccuracySpreadValue * distancefactor, +AccuracySpreadValue * distancefactor);
							float RandZSpread = FMath::RandRange(-AccuracySpreadValue * distancefactor, +AccuracySpreadValue * distancefactor);
							float RandXSpread = FMath::RandRange(-AccuracySpreadValue * distancefactor, +AccuracySpreadValue * distancefactor);
							//float VelocityFactor = GetVelocity().GetClampedToSize(1.0f, 2.0f).Size();
							FVector AccuracyChange = FVector(RandXSpread, RandYSpread, RandZSpread);
							//FPlane testplane = UKismetMathLibrary::MakePlaneFromPointAndNormal(hit.Location, hit.Normal);
							//UKismetSystemLibrary::DrawDebugPlane(World, testplane, hit.Location, 100, FColor::Blue, 100.0f);
							FVector TestBulletLocation = hit.Location + AccuracyChange;
							//DrawDebugPoint(World, TestBulletLocation, 5, FColor::Black, false, 10.0f);
							//DrawDebugLine(World, FPSCameraComponent->GetComponentLocation(), TestBulletLocation, FColor::Blue, false, 5.0f);
							FVector dirNormalized = FVector(FPSCameraComponent->GetComponentLocation() - TestBulletLocation).GetSafeNormal();


							FHitResult testhit;
							if (World->LineTraceSingleByChannel(testhit, FPSCameraComponent->GetComponentLocation(), FPSCameraComponent->GetComponentLocation() + (-dirNormalized * CurrentPrimary->Range), ECollisionChannel::ECC_Visibility, QueryParams))
							{
								DrawDebugLine(World, CurrentPrimary->MuzzleLocation->GetComponentLocation(), testhit.Location, FColor::Black, false, 2.0f);
								UE_LOG(LogClass, Log, TEXT("We hit %s"), *testhit.GetActor()->GetName());
								if (AFPSCharacter* const hitplayer = Cast<AFPSCharacter>(testhit.GetActor())) {
									UE_LOG(LogClass, Log, TEXT("Distance: %f"), FVector::Dist(testhit.Location, CurrentPrimary->MuzzleLocation->GetComponentLocation()));
									float ShotDistance = FVector::Dist(testhit.Location, CurrentPrimary->MuzzleLocation->GetComponentLocation());
									float DamagePercent = 0;
									if (ShotDistance <= CurrentPrimary->PreferredRange)
									{
										if (testhit.BoneName == FName("head"))
										{
											UE_LOG(LogClass, Log, TEXT("HEADSHOT"));
											DamagePercent = CurrentPrimary->HeadShotIncrease;
										}
										else
										{
											DamagePercent = 1.0f;
										}
									}
									else
									{
										float WeaponRangeDifference = CurrentPrimary->Range - CurrentPrimary->PreferredRange;
										float ShotRangeDifference = CurrentPrimary->Range - ShotDistance;
										float OppositePercentToZeroDamage = ShotRangeDifference / WeaponRangeDifference;
										if (testhit.BoneName == FName("head"))
										{
											UE_LOG(LogClass, Log, TEXT("HEADSHOT"));
											DamagePercent = roundf(OppositePercentToZeroDamage * CurrentPrimary->HeadShotIncrease * 100) / 100;
										}
										else
										{
											DamagePercent = roundf(OppositePercentToZeroDamage * 100) / 100;

										}
									}
									UE_LOG(LogClass, Log, TEXT("DamagePercent: %f"), DamagePercent);
									hitplayer->Shooter = this;

									hitplayer->ServerChangeHealthBy(-CurrentPrimary->BulletDamage * DamagePercent);
									hitplayer->TriggerUpdateUI();
									//UE_LOG(LogClass, Log, FString::SanitizeFloat(hitplayer->GetCurrentHealth());
									//hitplayer->HealthPercentage = hitplayer->GetCurrentHealth() / hitplayer->GetInitialHealth();
									GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::SanitizeFloat(hitplayer->GetCurrentHealth()));
									GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::SanitizeFloat(hitplayer->HealthPercentage));
									//UE_LOG(LogClass, Log, TEXT("%s health is now %s"), *hitplayer->GetName(), FString::SanitizeFloat(hitplayer->GetCurrentHealth()));
								}
							}
						}
						else
						{
							//DrawDebugLine(World, CurrentPrimary->MuzzleLocation->GetComponentLocation(), FPSCameraComponent->GetComponentLocation() + (FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range), FColor::Red, false, 3.0f);
							float distancefactor = CurrentPrimary->Range / 1000.0f;
							float RandYSpread = FMath::RandRange(-AccuracySpreadValue * distancefactor, +AccuracySpreadValue * distancefactor);
							float RandZSpread = FMath::RandRange(-AccuracySpreadValue * distancefactor, +AccuracySpreadValue * distancefactor);
							float RandXSpread = FMath::RandRange(-AccuracySpreadValue * distancefactor, +AccuracySpreadValue * distancefactor);
							//float VelocityFactor = GetVelocity().GetClampedToSize(1.0f, 2.0f).Size();
							FVector AccuracyChange = FVector(RandXSpread, RandYSpread, RandZSpread);
							//FPlane testplane = UKismetMathLibrary::MakePlaneFromPointAndNormal(hit.Location, hit.Normal);
							//UKismetSystemLibrary::DrawDebugPlane(World, testplane, hit.Location, 100, FColor::Blue, 100.0f);
							FVector TestBulletLocation = FPSCameraComponent->GetComponentLocation() + (FPSCameraComponent->GetForwardVector() * CurrentPrimary->Range) + AccuracyChange;

							//DrawDebugLine(World, FPSCameraComponent->GetComponentLocation(), TestBulletLocation, FColor::Blue, false, 5.0f);
							FVector dirNormalized = FVector(FPSCameraComponent->GetComponentLocation() - TestBulletLocation).GetSafeNormal();


							FHitResult testhit;
							if (World->LineTraceSingleByChannel(testhit, FPSCameraComponent->GetComponentLocation(), FPSCameraComponent->GetComponentLocation() + (-dirNormalized * CurrentPrimary->Range), ECollisionChannel::ECC_Visibility, QueryParams))
							{
								//DrawDebugPoint(World, TestBulletLocation, 5, FColor::Black, false, 10.0f);
								DrawDebugLine(World, CurrentPrimary->MuzzleLocation->GetComponentLocation(), testhit.Location, FColor::Black, false, 2.0f);
								UE_LOG(LogClass, Log, TEXT("We hit %s"), *testhit.GetActor()->GetName());
								if (AFPSCharacter* const hitplayer = Cast<AFPSCharacter>(testhit.GetActor())) {
									UE_LOG(LogClass, Log, TEXT("Distance: %f"), FVector::Dist(testhit.Location, CurrentPrimary->MuzzleLocation->GetComponentLocation()));
									float ShotDistance = FVector::Dist(testhit.Location, CurrentPrimary->MuzzleLocation->GetComponentLocation());
									float DamagePercent = 0;
									if (ShotDistance <= CurrentPrimary->PreferredRange)
									{
										if (testhit.BoneName == FName("head"))
										{
											UE_LOG(LogClass, Log, TEXT("HEADSHOT"));
											DamagePercent = CurrentPrimary->HeadShotIncrease;
										}
										else
										{
											DamagePercent = 1.0f;
										}
									}
									else
									{
										float WeaponRangeDifference = CurrentPrimary->Range - CurrentPrimary->PreferredRange;
										float ShotRangeDifference = CurrentPrimary->Range - ShotDistance;
										float OppositePercentToZeroDamage = ShotRangeDifference / WeaponRangeDifference;
										if (testhit.BoneName == FName("head"))
										{
											UE_LOG(LogClass, Log, TEXT("HEADSHOT"));
											DamagePercent = roundf(OppositePercentToZeroDamage * CurrentPrimary->HeadShotIncrease * 100) / 100;
										}
										else
										{
											DamagePercent = roundf(OppositePercentToZeroDamage * 100) / 100;

										}
									}
									hitplayer->ServerChangeHealthBy(-CurrentPrimary->BulletDamage * DamagePercent);

									UE_LOG(LogClass, Log, TEXT("DamagePercent: %f"), DamagePercent);
									hitplayer->Shooter = this;


									hitplayer->TriggerUpdateUI();
									//UE_LOG(LogClass, Log, FString::SanitizeFloat(hitplayer->GetCurrentHealth());
									//hitplayer->HealthPercentage = hitplayer->GetCurrentHealth() / hitplayer->GetInitialHealth();
									GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::SanitizeFloat(hitplayer->GetCurrentHealth()));
									GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::SanitizeFloat(hitplayer->HealthPercentage));
									//UE_LOG(LogClass, Log, TEXT("%s health is now %s"), *hitplayer->GetName(), FString::SanitizeFloat(hitplayer->GetCurrentHealth()));
								}
							}
						}


					}
					CurrentPrimary->ChangeAmmo(CurrentPrimary->TotalAmmo, CurrentPrimary->AmmoLeftInMag - 1);
					UE_LOG(LogClass, Log, TEXT("IsZoomed? %s"), (IsZoomed ? TEXT("True") : TEXT("False")));
					if (IsZoomed == true)
					{
						if (AccuracySpreadValue <= CurrentPrimary->ZoomMaxSpread)
						{
							AccuracySpreadValue += CurrentPrimary->ZoomAccuracySpreadIncrease;
							UE_LOG(LogClass, Log, TEXT("Zoomaccuracyspreadincrease: %f"), CurrentPrimary->ZoomAccuracySpreadIncrease);
						}
					}
					else
					{
						if (AccuracySpreadValue <= CurrentPrimary->MaxSpread)
						{
							AccuracySpreadValue += CurrentPrimary->AccuracySpreadIncrease;
							UE_LOG(LogClass, Log, TEXT("accuracyspreadincrease: %f"), CurrentPrimary->AccuracySpreadIncrease);
						}
					}
				}
				if (CurrentPrimary->AmmoLeftInMag <= 0 && CurrentPrimary->TotalAmmo > 0)
				{
					ServerReload();
				}
			}
		}
	}

}


// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentState = EPlayerState::EPlayerPlaying;

	switch (GetCurrentState())
	{
	case EPlayerState::EPlayerWaiting:

		break;
	case EPlayerState::EPlayerPlaying:


		break;
	case EPlayerState::EPlayerDead:

		break;
	default:
	case EPlayerState::EPlayerUnknown:
		break;
	}
	if (GEngine)
	{
		// Put up a debug message for five seconds. The -1 "Key" value (first argument) indicates that we will never need to update or refresh this message.
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
	}
}


void AFPSCharacter::PickupWeapon()
{
	ServerPickupWeapon();
}
void AFPSCharacter::DropWeapon()
{
	ServerDropWeapon();
}


bool AFPSCharacter::ServerDropWeapon_Validate()
{
	return true;
}

void AFPSCharacter::ServerDropWeapon_Implementation()
{
	if (AFPSGameState* const GameState = Cast<AFPSGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->GetCurrentState() == EGamePlayState::EPlaying)
		{


			if (Role == ROLE_Authority)
			{
				if (CurrentPrimary != NULL)
				{
					if (AGun* Gun = Cast<AGun>(CurrentPrimary))
					{
						Gun->DroppedBy(this);
						CurrentPrimary = NULL;
					}
				}
			}
		}
	}
}

bool AFPSCharacter::ServerPickupWeapon_Validate()
{
	return true;
}

void AFPSCharacter::ServerPickupWeapon_Implementation()
{
	if (AFPSGameState* const GameState = Cast<AFPSGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->GetCurrentState() == EGamePlayState::EPlaying)
		{


			if (Role == ROLE_Authority)
			{
				// Get all overlapping actors and store them in an array
				TArray<AActor*> CollectedActors;
				CollectionBox->GetOverlappingActors(CollectedActors);

				for (int i = 0; i < CollectedActors.Num(); ++i)
				{
					AGun* const Gun = Cast<AGun>(CollectedActors[i]);
					if (Gun != NULL && !Gun->IsPendingKill() && Gun->IsActive()) {
						if (CurrentPrimary != NULL)
						{
							if (AGun* PrimaryGun = Cast<AGun>(CurrentPrimary))
							{


								if (Gun->GunName == PrimaryGun->GunName)
								{
									UE_LOG(LogClass, Log, TEXT("AlreadyHaveWeapon"));
									return;
								}
							}
						}
						CurrentPrimary = Gun;
						Gun->PickedUpBy(this);

						UE_LOG(LogClass, Log, TEXT("Current Primary: %s"), *CurrentPrimary->GetName());
						HasWeapon = true;

					}
				}
			}
		}
	}
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSCharacter, FPSMesh);
	DOREPLIFETIME(AFPSCharacter, FPSCameraComponent);
	DOREPLIFETIME(AFPSCharacter, CurrentPrimary);
	DOREPLIFETIME(AFPSCharacter, AccuracySpreadValue);
	DOREPLIFETIME(AFPSCharacter, IsZoomed);

}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	if (CurrentPrimary != NULL)
	{
		if (!IsZoomed)
		{
			if (AccuracySpreadValue > CurrentPrimary->MinSpread)
			{


				AccuracySpreadValue -= CurrentPrimary->AccuracySpreadDecrease;


			}
			if (AccuracySpreadValue < CurrentPrimary->MinSpread)
			{

				AccuracySpreadValue = CurrentPrimary->MinSpread;


			}
		}
		else
		{
			if (AccuracySpreadValue > CurrentPrimary->ZoomMinSpread)
			{


				AccuracySpreadValue -= CurrentPrimary->AccuracySpreadDecrease;


			}
			if (AccuracySpreadValue < CurrentPrimary->ZoomMinSpread)
			{

				AccuracySpreadValue = CurrentPrimary->ZoomMinSpread;


			}
		}
	}
	else
	{
		AccuracySpreadValue = 0.0f;
	}
	if (CurrentState == EPlayerState::EPlayerWaiting)
	{
		GetCharacterMovement()->DisableMovement();
	}
	if (Role == ROLE_AutonomousProxy)
	{
		if (GetController())
		{
			ServerUpdateRotation(FPSCameraComponent->GetComponentRotation());
		}

	}
	//WeaponHoldLocation->SetWorldLocation(FPSMesh->GetSocketLocation(FName(TEXT("WeaponLocation"))));
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up "movement" bindings.
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	// Set up "look" bindings.
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);

	// Set up "action" bindings.
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFPSCharacter::ServerReload);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AFPSCharacter::ServerOnZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AFPSCharacter::ServerOnStopZoom);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFPSCharacter::PickupWeapon);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AFPSCharacter::OnShoot);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AFPSCharacter::OnStopShoot);
}
bool AFPSCharacter::ServerReload_Validate()
{
	return true;
}

void AFPSCharacter::ServerReload_Implementation()
{
	if (CurrentPrimary != NULL)
	{
		if (!CurrentPrimary->IsReloading && CurrentPrimary->AmmoLeftInMag != CurrentPrimary->MagazineSize)
		{
			ServerOnStopShoot();
			CurrentPrimary->StartReload();
		}
	}
}
void AFPSCharacter::MoveForward(float Value)
{

	// Find out which way is "forward" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(FRotator(0, Controller->GetControlRotation().Yaw, 0)).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::MoveRight(float Value)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::StartJump()
{
	bPressedJump = true;
}

void AFPSCharacter::StopJump()
{
	bPressedJump = false;
}

