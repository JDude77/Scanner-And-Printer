// Fill out your copyright notice in the Description page of Project Settings.

#include "ScannableObject.h"

// Sets default values
AScannableObject::AScannableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AScannableObject::BeginPlay()
{
	Super::BeginPlay();
}

bool AScannableObject::Scan(float deltaTime)
{
	if (!hasBeenScannedBefore)
	{
		UE_LOG(LogTemp, Warning, TEXT("First Scan Time: %f"), (scanTime / maxScanTime) * 100);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Non-First Scan Time: %f"), (scanTime / minScanTime) * 100);
	}

	if ((scanTime < maxScanTime && !hasBeenScannedBefore) || (scanTime < minScanTime && hasBeenScannedBefore))
	{
		scanTime += deltaTime;
		finishedScanning = false;
	}// End if
	else if ((scanTime >= maxScanTime && !hasBeenScannedBefore) || (scanTime >= minScanTime && hasBeenScannedBefore))
	{
		finishedScanning = true;
	}// End else if

	return finishedScanning;
}

// Called every frame
void AScannableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isBeingScanned && !finishedScanning) Scan(DeltaTime);

	else if (!isBeingScanned && finishedScanning)
	{
		scanTime = 0.0f;
		if (finishedScanning)
		{
			hasBeenScannedBefore = true;
			finishedScanning = false;
		}//End if
	}//End else if
}

void AScannableObject::SetIsBeingScanned(bool beingScanned)
{
	isBeingScanned = beingScanned;
}

UStaticMesh* AScannableObject::GetMesh()
{
	return printableMesh;
}

float AScannableObject::GetHealthLostOnPrint()
{
	return healthLostOnPrint;
}

bool AScannableObject::GetFinishedScanning()
{
	return finishedScanning;
}

void AScannableObject::ResetScanTime()
{
	isBeingScanned = false;
	finishedScanning = false;
	scanTime = 0.0f;
}