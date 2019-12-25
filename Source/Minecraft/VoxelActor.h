// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "VoxelActor.generated.h"

UCLASS()
class MINECRAFT_API AVoxelActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxelActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray <UMaterialInterface *> Materials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
		int32 randomSeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
		int32 voxelSize = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
		int32 chunkLineElements = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
		int32 chunkXindex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
		int32 chunkYIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float xMult = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float yMult = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float zMult = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float weight = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float freq = 1;

	UPROPERTY()
		int32 chunkTotalElements;

	UPROPERTY()
		int32 chunkZElements;

	UPROPERTY()
		int32 chunkLineElementsP2;

	UPROPERTY()
		int32 voxelSizeHalf;

	UPROPERTY()
		TArray <int32> chunkFields;

	UPROPERTY()
		UProceduralMeshComponent* proceduralComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
};
