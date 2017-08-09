// Fill out your copyright notice in the Description page of Project Settings.

#include "AssaultRifle.h"
#include "FPSCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
//testing!!!!!!!!!!!!!!!!!!!!!

AAssaultRifle::AAssaultRifle()
{
	GunName = TEXT("Assault Rifle");
	//keep movement synced from server to clients
	bReplicates = true;
	bReplicateMovement = true;
	//this pickup is physics enabled and should move
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetEnableGravity(true);
	IsProjectile = false;
	IsAutomatic = true;
	IsBurst = false;
	FireRate = 0.1f;
	ReloadTime = 2.5f;
	TotalAmmo = 150;
	Range = 7000;
	HeadShotIncrease = 1.4f;
	ZoomFOV = 30.0f;
	ZoomRate = 0.01;
	RecoilValue = 2.5f;
	ZoomRecoilValue = 1.2f;
	BulletDamage = 5.0f;
	MagazineSize = 25;
	PreferredRange = 1800;
	AccuracySpreadIncrease = 10.0f;
	ZoomAccuracySpreadIncrease = 6.0f;
	MaxSpread = 60.0f;
	MinSpread = 20.0f;
	ZoomMaxSpread = 30.0f;
	ZoomMinSpread = 10.0f;
	AccuracySpreadDecrease = 3.0f;
	AmmoLeftInMag = MagazineSize;
	IsExplosive = false;
}

void AAssaultRifle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

/*
void AAssaultRifle::PickedUpBy(APawn * Pawn)
{
Super::PickedUpBy(Pawn);


}
*/
