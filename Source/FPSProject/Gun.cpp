// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "Net/UnrealNetwork.h"


AGun::AGun()
{
	//tell engine to replicate this actor
	bReplicates = true;
	IsExplosive = false;
	CanFire = true;
	IsReloading = false;
	IsProjectile = false;
	TotalAmmo = 0;
	AmmoLeftInMag = 0;
	MagazineSize = 0;
	//gun doesn't need to tick
	PrimaryActorTick.bCanEverTick = false;
	//static mesh actor disables overlap events by default, which we need to re-enable
	GetStaticMeshComponent()->bGenerateOverlapEvents = true;
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(RootComponent);
	if (Role == ROLE_Authority)
	{
		bIsActive = true;
	}
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGun, bIsActive);
	DOREPLIFETIME(AGun, AmmoLeftInMag);
	DOREPLIFETIME(AGun, TotalAmmo);
	DOREPLIFETIME(AGun, MagazineSize);
	DOREPLIFETIME(AGun, CanFire);
	DOREPLIFETIME(AGun, IsReloading);
}

bool AGun::IsActive()
{
	
	return bIsActive;
}

bool AGun::StartReload_Validate()
{
	return true;
}
bool AGun::EndReload_Validate()
{
	return true;
}

bool AGun::ChangeAmmo_Validate(int32 Ammo, int32 Mag)
{
	return true;
}

void AGun::ChangeAmmo_Implementation(int32 Ammo, int32 Mag)
{
	if (Role == ROLE_Authority)
	{
		UE_LOG(LogClass, Log, TEXT("ChangedAmmoFrom - Server"));
	}
	else if (Role == ROLE_AutonomousProxy)
	{
		UE_LOG(LogClass, Log, TEXT("ChangedAmmoFrom - Owner"));
	}
	else
	{
		UE_LOG(LogClass, Log, TEXT("ChangedAmmoFrom - Unknown"));
	}
	AmmoLeftInMag = Mag;
	TotalAmmo = Ammo;
	
	//do something
}


void AGun::SetActive(bool NewWeaponState)
{
	if (Role == ROLE_Authority)
	{
		bIsActive = NewWeaponState;
	}
}

void AGun::WasCollected_Implementation()
{
	// log a debug message
	//UE_LOG(LogClass, Log, TEXT("AGun::WasCollected_Implementation %s"), *GetName());
}

bool AGun::SetCanFire_Validate(bool NewCanFire)
{
	return true;
}


void AGun::SetCanFire_Implementation(bool NewCanFire)
{
	CanFire = NewCanFire;
}
bool AGun::SetIsReloading_Validate(bool NewReloading)
{
	return true;
}


void AGun::SetIsReloading_Implementation(bool NewReloading)
{
	IsReloading = NewReloading;
}

void AGun::ClientStartReload()
{
	

	
	
	//StartReload();
	
}
void AGun::ClientEndReload()
{

}

void AGun::StartReload_Implementation()
{
	if (!IsReloading)
	{
		if (TotalAmmo > 0) {
			SetCanFire(false);
			GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &AGun::EndReload, ReloadTime, false);
			SetIsReloading(true);
		}
	}
	else
	{
		UE_LOG(LogClass, Log, TEXT("AlreadyReloading"));
	}
}
void AGun::EndReload_Implementation()
{

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	if (Role == ROLE_Authority)
	{
		UE_LOG(LogClass, Log, TEXT("SERVER RELOAD"));

	//ClientStartReload();
	
	
	}
	if (AmmoLeftInMag > 0)
	{
		if (TotalAmmo + AmmoLeftInMag >= MagazineSize)
		{
			ChangeAmmo(TotalAmmo + AmmoLeftInMag - MagazineSize, MagazineSize);
		}
		else
		{
			ChangeAmmo(0, TotalAmmo + AmmoLeftInMag);
		}
	}
	else
	{
		if (TotalAmmo >= MagazineSize)
		{
			ChangeAmmo(TotalAmmo - MagazineSize, MagazineSize);
		}
		else
		{
			ChangeAmmo(0, TotalAmmo);
		}
	}
	//ChangeAmmo(TotalAmmo, MagazineSize);
	SetIsReloading(false);
	SetCanFire(true);
	
	UE_LOG(LogClass, Log, TEXT("EndedReload"));
	
}
void AGun::DroppedBy(APawn * Pawn)
{
	
	if (Role == ROLE_Authority)
	{
		ClientOnDroppedBy(Pawn);
		DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		GetStaticMeshComponent()->SetSimulatePhysics(true);
	}
}

void AGun::PickedUpBy(APawn * Pawn)
{
	if (Role == ROLE_Authority)
	{
		//store the pawn who picked up the pickup
		WeaponInstigator = Pawn;
		//Notify clients of the picked up action
		ClientOnPickedUpBy(Pawn);

		if (AFPSCharacter* const AttachedPlayer = Cast<AFPSCharacter>(Pawn)) {
			UE_LOG(LogClass, Log, TEXT("%s was picked up by %s"), *GetName(), *Pawn->GetName());
			GetStaticMeshComponent()->SetSimulatePhysics(false);
			//this->AttachRootComponentTo(AttachedPlayer->FPSMesh, FName(TEXT("WeaponLocation")),EAttachLocation::SnapToTarget);
			AttachToComponent(AttachedPlayer->FPSMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("WeaponLocation")));

			//this->AttachRootComponentTo(AttachedPlayer->FPSMesh, FName(TEXT("WeaponLocation")));
			//this->AttachRootComponentTo(atta)
			//FRotator MyRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AttachedPlayer->FPSCameraComponent->GetComponentLocation() + AttachedPlayer->FPSCameraComponent->GetForwardVector() * 200);
			//SetActorRotation(FRotator(MyRotation.Pitch,MyRotation.Yaw,MyRotation.Roll));
			//SetActorRotation(MyRotation);

		}
		// give clients time to play vfx, etc... before destroying the battery
		//SetLifeSpan(2.0f);

	}
}

void AGun::OnRep_IsActive()
{

}

void AGun::ClientOnPickedUpBy_Implementation(APawn* Pawn)
{
	//store the pawn who picked up pickup (client
	WeaponInstigator = Pawn;
	//fire the blueprint native event, which itself cannot be replicated
	WasCollected();
}
void AGun::ClientOnDroppedBy_Implementation(APawn* Pawn)
{

	WeaponInstigator = NULL;

}