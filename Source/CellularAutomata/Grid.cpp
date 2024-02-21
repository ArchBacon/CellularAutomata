// Fill out your copyright notice in the Description page of Project Settings.

#include "Grid.h"
#include "Components/InstancedStaticMeshComponent.h"

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
	Step();
	RenderInstancedCells();
}

void AGrid::Finalize()
{
	for (int i = 0; i < 15; i++)
	{
		Step();
	}
	
	RenderInstancedCells();
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

void AGrid::RenderDefault()
{
	RenderInstancedCells();
}

void AGrid::RenderInverted()
{
	InstancedStaticMeshComponent->ClearInstances();
	for (int y = 0; y < MapSize.Y; y++)
	{
		for (int x = 0; x < MapSize.X; x++)
		{
			const int Index = y * MapSize.X + x;
			if (Map[Index] == 1)
			{
				continue;
			}
		
			FTransform Transform(FVector(x * 100, y * 100, 0));
			InstancedStaticMeshComponent->AddInstance(Transform);
		}
	}
}

int AGrid::CountCellWalls(const FVector2D Coordinate)
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
		const auto Cell = Coordinate + Neighbour;
		if (!IsValidCoordinate(Cell))
		{
			WallCount += 1;
		}
		else
		{
			const int Index = Cell.Y * MapSize.X + Cell.X;
			if (Map[Index] == 1)
			{
				WallCount += 1;
			}
		}
	}
	
	return WallCount;
}

bool AGrid::IsValidCoordinate(const FVector2D Coordinate) const
{
	return Coordinate.X >= 0 && Coordinate.X < MapSize.X
		&& Coordinate.Y >= 0 && Coordinate.Y < MapSize.Y;
}

void AGrid::Step()
{
	TArray<int> NewMap = Map;
	for (int y = 0; y < MapSize.Y; y++)
	{
		for (int x = 0; x < MapSize.X; x++)
		{
			const int Index = y * MapSize.X + x;
			const int Count = CountCellWalls(FVector2D(x, y));
			if (Map[Index] == 1)
			{
				NewMap[Index] = !(Count < DeathLimit);
			}
			else
			{
				NewMap[Index] = Count > BirthLimit;
			}
		}
	}

	Map = NewMap;
}
