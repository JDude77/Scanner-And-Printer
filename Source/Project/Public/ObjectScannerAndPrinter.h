// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ScannableObject.h"
#include "ObjectScannerAndPrinter.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_API UObjectScannerAndPrinter : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UObjectScannerAndPrinter();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	#pragma region Blueprint-Callable Variables

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxDistance = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool printUnlocked = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool scanUnlocked = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int maxScannedObjects = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UStaticMesh*, float> scannedObjects;

	UPROPERTY(BlueprintReadOnly)
	AScannableObject* hitObject;

	UPROPERTY(BlueprintReadOnly)
	bool isScanning = false;

	UPROPERTY(BlueprintReadOnly)
	bool isPrinting = false;

	UPROPERTY(BlueprintReadOnly)
	float printTime = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool objectPrinted = false;

	UPROPERTY(BlueprintReadOnly)
	bool canPrint = false;

	UPROPERTY(BlueprintReadOnly)
	bool canScan = false;

	UPROPERTY(BlueprintReadOnly)
	int scannedObjectIndex = 0;
	#pragma endregion

	#pragma region Non-Blueprint-Callable Variables

	bool objectAddedFromScan = false;

	AActor* hitActor;

	#pragma endregion

	#pragma region Blueprint-Callable Functions

	UFUNCTION(BlueprintCallable)
	void StartScan();

	UFUNCTION(BlueprintCallable)
	void StopScan();

	UFUNCTION(BlueprintCallable)
	void StartPrint();

	UFUNCTION(BlueprintCallable)
	void StopPrint();

	UFUNCTION(BlueprintCallable)
	void UpdateSelectedItem(int change);

	UFUNCTION(BlueprintCallable)
	float UpdateHealth(float originalHealth, float changeAmount);
	#pragma endregion

	#pragma region Non-Blueprint-Callable Functions

	void AddObjectToPrinter();

	bool LineTrace(FHitResult& outHitResult);

	bool CheckIfCanScan(FHitResult& hitResult);

	bool CheckIfCanPrint(FHitResult& hitResult);

	void ProcessPrintUnlock(FHitResult& hitResult);

	void ProcessScanUnlock(FHitResult& hitResult);

	void ProcessPrintingObject(float deltaTime, FHitResult& hitResult);

	void ProcessScanningObject();

	void PrintObject(FVector position);

	#pragma endregion

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
