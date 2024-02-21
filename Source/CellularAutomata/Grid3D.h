// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid3D.generated.h"

UCLASS(Blueprintable, BlueprintType)
class CELLULARAUTOMATA_API AGrid3D : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent;
	
	UPROPERTY()
	TArray<int> Map;

	UPROPERTY(EditAnywhere)
	FVector MapSize = {64, 64, 64};

	UPROPERTY(EditAnywhere)
	int Seed = 1337;

	UPROPERTY(EditAnywhere)
	int DeathLimit = 10;
	
	UPROPERTY(EditAnywhere)
	int BirthLimit = 13;

	UPROPERTY(EditAnywhere)
	float AliveChance = 0.45f;

	UPROPERTY(EditAnywhere)
	int Slices = 1;
	
	FRandomStream Random;
	
public:	
	AGrid3D();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<int>& GetMap() const { return Map; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FVector& GetMapSize() const { return MapSize; }

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Grid 3D")
	void StepIteration();

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Grid 3D")
	void Finalize();

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Grid 3D")
	void RenderSlices();

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Grid 3D")
	void RenderDefault();

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Grid 3D")
	void RenderInverted();
	
	UFUNCTION(BlueprintCallable)
	void RenderInstancedCells();
	
protected:
	UFUNCTION()
	int CountCellWalls(FVector Coordinate);

	UFUNCTION()
	bool IsValidCoordinate(const FVector& Coordinate) const;

	UFUNCTION()
	void Step();
};
