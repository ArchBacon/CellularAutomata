// Fill out your copyright notice in the Description page of Project Settings.

#include "Grid.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/Engine.h"

AGrid::AGrid()
{
	PrimaryActorTick.bCanEverTick = false;

	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Instanced Static Mesh"));
	InstancedStaticMeshComponent->SetupAttachment(RootComponent);
}

void AGrid::BeginPlay()
{
	Super::BeginPlay();

	Random.Initialize(Seed);
	Map.Init(1, MapSize.X * MapSize.Y);

	for (int y = 0; y < MapSize.Y; y++)
	{
		for (int x = 0; x < MapSize.X; x++)
		{
			Map[y * MapSize.X + x] = static_cast<int>(Random.FRand() < AliveChance);
		}
	}
	
	RenderInstancedCells();
}

void AGrid::StepIteration()
{
	TArray<int> newMap = Map;
	for (int y = 0; y < MapSize.Y; y++)
	{
		for (int x = 0; x < MapSize.X; x++)
		{
			const int Index = y * MapSize.X + x;
			const int Count = CountCellWalls(FVector2D(x, y));
			if (Map[Index] == 1)
			{
				newMap[Index] = !(Count < DeathLimit);
			}
			else
			{
				newMap[Index] = Count > BirthLimit;
			}
		}
	}

	Map = newMap;
	RenderInstancedCells();
}

void AGrid::Finalize()
{
	for (int i = 0; i < 15; i++)
	{
		StepIteration();
	}
}

void AGrid::RenderInstancedCells()
{
	InstancedStaticMeshComponent->ClearInstances();
	for (int y = 0; y < MapSize.Y; y++)
	{
		for (int x = 0; x < MapSize.X; x++)
		{
			const int Index = y * MapSize.X + x;
			if (Map[Index] == 0)
			{
				continue;
			}
		
			FTransform Transform(FVector(x * 100, y * 100, 0));
			InstancedStaticMeshComponent->AddInstance(Transform);
		}
	}
}

int AGrid::CountCellWalls(FVector2D Coord)
{
	int WallCount = 0;
	TArray Neighbours = {
		FVector2D(-1, 1),
		FVector2D(0, 1),
		FVector2D(1, 1),
		FVector2D(-1, 0),
		FVector2D(1, 0),
		FVector2D(-1, -1),
		FVector2D(0, -1),
		FVector2D(1, -1),
	};

	for (auto Neighbour : Neighbours)
	{
		const auto Cell = Coord + Neighbour;
		if (!IsValidCoordinate(Cell))
		{
			WallCount += 1;
		}
		else
		{
			int Index = Cell.Y * MapSize.X + Cell.X;
			int Value = Map[Index];
			if (Value == 1)
			{
				WallCount += 1;
			}
		}
	}
	
	return WallCount;
}

bool AGrid::IsValidCoordinate(const FVector2D Coord) const
{
	return
		Coord.X >= 0 && Coord.X < MapSize.X
	 && Coord.Y >= 0 && Coord.Y < MapSize.Y;
}
