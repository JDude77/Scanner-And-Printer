// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ScannableObject.generated.h"

UCLASS()
class PROJECT_API AScannableObject : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScannableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool hasBeenScannedBefore = false;

	bool Scan(float deltaTime);

	UPROPERTY(BlueprintReadOnly)
	bool finishedScanning = false;

	UPROPERTY(BlueprintReadOnly)
	bool isBeingScanned = false;

	UPROPERTY(BlueprintReadOnly)
	float scanTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxScanTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float minScanTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* printableMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float healthLostOnPrint = 5.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetIsBeingScanned(bool beingScanned);

	UStaticMesh* GetMesh();

	float GetHealthLostOnPrint();

	bool GetFinishedScanning();

	void ResetScanTime();
};
