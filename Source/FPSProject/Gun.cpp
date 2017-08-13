// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "Net/UnrealNetwork.h"


AGun::AGun()
{
	//tell engine to replicate this actor
	bReplicates = true;
	IsExplosive = false;
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
}

bool AGun::IsActive()
{
	
	return bIsActive;
}

void AGun::ChangeAmmo_Implementation(int32 Ammo, int32 Mag)
{
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