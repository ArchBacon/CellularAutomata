// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"

UCLASS(Blueprintable, BlueprintType)
class CELLULARAUTOMATA_API AGrid : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent;
	
	UPROPERTY()
	TArray<int> Map;

	UPROPERTY(EditAnywhere)
	FVector2D MapSize = {64, 64};

	UPROPERTY(EditAnywhere)
	int Seed = 1337;

	UPROPERTY(EditAnywhere)
	int DeathLimit = 3;
	
	UPROPERTY(EditAnywhere)
	int BirthLimit = 4;

	UPROPERTY(EditAnywhere)
	float AliveChance = 0.45f;
	
	FRandomStream Random;
	
public:	
	AGrid();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<int>& GetMap() const { return Map; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FVector2D& GetMapSize() const { return MapSize; }

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Grid")
	void StepIteration();

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Grid")
	void Finalize();

	UFUNCTION(BlueprintCallable)
	void RenderInstancedCells();
	
protected:
	UFUNCTION()
	int CountCellWalls(FVector2D Coord);

	UFUNCTION()
	bool IsValidCoordinate(FVector2D Coord) const;
};
