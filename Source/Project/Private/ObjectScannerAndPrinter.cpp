// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectScannerAndPrinter.h"
#include "ScannableObject.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UObjectScannerAndPrinter::UObjectScannerAndPrinter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	scannedObjects = TMap<UStaticMesh*, float>();
	// ...
}


// Called when the game starts
void UObjectScannerAndPrinter::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UObjectScannerAndPrinter::StartScan()
{
	if (canScan)
	{
		isScanning = true;
		hitObject->SetIsBeingScanned(true);
	}//End if
}

void UObjectScannerAndPrinter::StopScan()
{
	isScanning = false;
	if (hitObject != nullptr) hitObject->SetIsBeingScanned(false);
}

void UObjectScannerAndPrinter::StartPrint()
{
	if (canPrint)
	{
		isPrinting = true;
	}
}

void UObjectScannerAndPrinter::StopPrint()
{
	isPrinting = false;
}

void UObjectScannerAndPrinter::UpdateSelectedItem(int change)
{
	if (!isPrinting)
	{
		scannedObjectIndex += change;

		TArray<UStaticMesh*> temp;
		scannedObjects.GenerateKeyArray(temp);

		if (temp.Num() >= maxScannedObjects)
		{
			if (scannedObjectIndex < 0) scannedObjectIndex = maxScannedObjects - 1;
			else if (scannedObjectIndex >= maxScannedObjects) scannedObjectIndex = 0;
		}//End if
		else if (temp.Num() > 0 && temp.Num() < maxScannedObjects)
		{
			if (scannedObjectIndex < 0) scannedObjectIndex = temp.Num() - 1;
			else if (scannedObjectIndex >= temp.Num()) scannedObjectIndex = 0;

		}//End else if
		else scannedObjectIndex = 0;
	}//End if
}

float UObjectScannerAndPrinter::UpdateHealth(float originalHealth, float changeAmount)
{
	originalHealth += changeAmount;
	return originalHealth;
}

void UObjectScannerAndPrinter::AddObjectToPrinter()
{
	TTuple<UStaticMesh*, float> newObject;
	newObject.Key = reinterpret_cast<UStaticMesh*>(hitObject->GetMesh());
	newObject.Value = hitObject->GetHealthLostOnPrint();

	if (scannedObjects.Contains(newObject.Key))
	{
		return;
	}

	else if (scannedObjects.Num() == maxScannedObjects)
	{
		TArray<UStaticMesh*> temp;
		scannedObjects.GenerateKeyArray(temp);
		for (int i = 0; i < temp.Num(); i++)
		{
			if (temp[i] == temp[scannedObjectIndex])
			{
				UStaticMesh* key = temp[scannedObjectIndex];
				scannedObjects.Remove(key);
				break;
			}//End if
		}//End for
	}// End if

	scannedObjects.Add(newObject);
}

bool UObjectScannerAndPrinter::LineTrace(FHitResult& outHitResult)
{
	// Send line trace out in front of gun to check for scannable object
	FVector hitStart = GetComponentLocation();
	FVector hitEnd = (GetForwardVector() * maxDistance) + hitStart;
	FCollisionQueryParams collisionParameters;

	DrawDebugLine(GetWorld(), hitStart, hitEnd, FColor(200, 100, 100));

	return GetWorld()->LineTraceSingleByChannel(outHitResult, hitStart, hitEnd, ECC_Visibility, collisionParameters);
}

bool UObjectScannerAndPrinter::CheckIfCanScan(FHitResult& hitResult)
{
	if (LineTrace(hitResult))
	{
		if (hitResult.GetActor()->Tags.Contains("Scannable"))
		{
			return true;
		}//End if
	}//End if

	return false;
}

bool UObjectScannerAndPrinter::CheckIfCanPrint(FHitResult& hitResult)
{
	// Make sure there's at least one printable object scanned and in the array
	if (scannedObjects.Num() != 0)
	{
		if (LineTrace(hitResult))
		{
			if (hitResult.GetActor()->Tags.Contains("Printable Surface"))
			{
				return true;
			}//End if
		}//End if
	}//End if

	return false;
}

void UObjectScannerAndPrinter::PrintObject(FVector position)
{
	FActorSpawnParameters spawnParams;
	spawnParams.Name = "PrintedObject";

	//Create new static mesh actor from scanned object
	AStaticMeshActor* newObject = GetWorld()->SpawnActor<AStaticMeshActor>(FActorSpawnParameters());
	newObject->SetMobility(EComponentMobility::Movable);
	newObject->SetActorLocationAndRotation(position, FRotator(0.0f, GetOwner()->GetActorRotation().Yaw + 180.0f, 0.0f));
	TArray<UStaticMesh*> temp;
	scannedObjects.GenerateKeyArray(temp);
	UStaticMesh* mesh = temp[scannedObjectIndex];
	newObject->GetStaticMeshComponent()->SetStaticMesh(mesh);
	UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	//scannedObjects[mesh]
}

// Called every frame
void UObjectScannerAndPrinter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult hitResult;

	ProcessScanUnlock(hitResult);

	ProcessScanningObject();
	
	ProcessPrintUnlock(hitResult);

	ProcessPrintingObject(DeltaTime, hitResult);
	
	if (!canPrint && !canScan)
	{
		hitActor = nullptr;
		hitObject = nullptr;
	}//End if
}

void UObjectScannerAndPrinter::ProcessPrintUnlock(FHitResult& hitResult)
{
	if (printUnlocked)
	{
		canPrint = CheckIfCanPrint(hitResult);
		if (canPrint)
		{
			hitActor = hitResult.GetActor();
		}//End if
		else if (!canPrint && hitActor != nullptr)
		{
			printTime = 0.0f;
		}
	}//End if

	 //Make sure that if the override is locking, it's never saying it can print
	else canPrint = false;
}

void UObjectScannerAndPrinter::ProcessPrintingObject(float deltaTime, FHitResult& hitResult)
{
	if (isPrinting && !objectPrinted)
	{
		if (hitActor != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Print Time: %f"), (printTime / scannedObjects.begin().Value()) * 100);
			printTime += deltaTime;
			TArray<float> temp;
			scannedObjects.GenerateValueArray(temp);
			if (printTime >= temp[scannedObjectIndex])
			{
				PrintObject(hitResult.Location);
				objectPrinted = true;
			}//End if
		}
	}//End if

	if (!isPrinting && objectPrinted) objectPrinted = false;
	else if (!isPrinting && hitActor != nullptr) printTime = 0.0f;
}

void UObjectScannerAndPrinter::ProcessScanningObject()
{
	if (isScanning && !objectAddedFromScan)
	{
		if (hitObject != nullptr)
		{
			if (hitObject->GetFinishedScanning())
			{
				AddObjectToPrinter();
				objectAddedFromScan = true;
			}//End if
		}//End if
	}//End if

	if (!isScanning && objectAddedFromScan) objectAddedFromScan = false;
	else if (!isScanning && hitObject != nullptr) hitObject->ResetScanTime();
}

void UObjectScannerAndPrinter::ProcessScanUnlock(FHitResult& hitResult)
{
	if (scanUnlocked)
	{
		canScan = CheckIfCanScan(hitResult);
		if (canScan)
		{
			hitObject = Cast<AScannableObject>(hitResult.GetActor());
		}
		else if (!canScan && hitObject != nullptr)
		{
			hitObject->ResetScanTime();
			hitObject = nullptr;
		}
		else hitObject = nullptr;
	}//End if

	 //Make sure that if the override is locking, it's never saying it can scan
	else
	{
		hitObject = nullptr;
		canScan = false;
	}//End else
}
