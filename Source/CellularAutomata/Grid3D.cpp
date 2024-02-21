// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid3D.h"
#include "Components/InstancedStaticMeshComponent.h"

AGrid3D::AGrid3D()
{
	PrimaryActorTick.bCanEverTick = false;

	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Instanced Static Mesh"));
	InstancedStaticMeshComponent->SetupAttachment(RootComponent);
}

void AGrid3D::BeginPlay()
{
	Super::BeginPlay();

	Random.Initialize(Seed);
	Map.Init(1, MapSize.X * MapSize.Y * MapSize.Z);

	for (int z = 0; z < MapSize.Z; z++)
	{
		for (int y = 0; y < MapSize.Y; y++)
		{
			for (int x = 0; x < MapSize.X; x++)
			{
				Map[z * MapSize.Y * MapSize.X + y * MapSize.X + x] = static_cast<int>(Random.FRand() < AliveChance);
			}
		}
	}

	RenderInstancedCells();
}

void AGrid3D::StepIteration()
{
	Step();
	RenderInstancedCells();
}

void AGrid3D::Finalize()
{
	for (int i = 0; i < 15; i++)
	{
		Step();
	}
	
	RenderInstancedCells();
}
void AGrid3D::RenderSlices()
{
	int MaxX = std::min((int)MapSize.X, Slices);
	
	InstancedStaticMeshComponent->ClearInstances();
	for (int z = 0; z < MapSize.Z; z++)
	{
		for (int y = 0; y < MapSize.Y; y++)
		{
			for (int x = 0; x < MaxX; x++)
			{
				const int Index = z * MapSize.Y * MapSize.X + y * MapSize.X + x;
				if (Map[Index] == 0)
				{
					continue;
				}

				FTransform Transform(FVector(x * 100, y * 100, z * 100));
				InstancedStaticMeshComponent->AddInstance(Transform);
			}
		}
	}
}

void AGrid3D::RenderDefault()
{
	RenderInstancedCells();
}

void AGrid3D::RenderInverted()
{
	InstancedStaticMeshComponent->ClearInstances();
	for (int z = 0; z < MapSize.Z; z++)
	{
		for (int y = 0; y < MapSize.Y; y++)
		{
			for (int x = 0; x < MapSize.X; x++)
			{
				const int Index = z * MapSize.Y * MapSize.X + y * MapSize.X + x;
				if (Map[Index] == 1)
				{
					continue;
				}

				FTransform Transform(FVector(x * 100, y * 100, z * 100));
				InstancedStaticMeshComponent->AddInstance(Transform);
			}
		}
	}
}

void AGrid3D::RenderInstancedCells()
{
	InstancedStaticMeshComponent->ClearInstances();
	for (int z = 0; z < MapSize.Z; z++)
	{
		for (int y = 0; y < MapSize.Y; y++)
		{
			for (int x = 0; x < MapSize.X; x++)
			{
				const int Index = z * MapSize.Y * MapSize.X + y * MapSize.X + x;
				if (Map[Index] == 0)
				{
					continue;
				}

				FTransform Transform(FVector(x * 100, y * 100, z * 100));
				InstancedStaticMeshComponent->AddInstance(Transform);
			}
		}
	}
}

int AGrid3D::CountCellWalls(FVector Coordinate)
{
	int WallCount = 0;
	TArray<FVector> Neighbours = {
		// Roof
		{-1, 1, 1},
		{0, 1, 1},
		{1, 1, 1},
		{-1, 0, 1},
		{0, 0, 1},
		{1, 0, 1},
		{-1, -1, 1},
		{0, -1, 1},
		{1, -1, 1},
		// Same
		{-1, 1, 0},
		{0, 1, 0},
		{1, 1, 0},
		{-1, 0, 0},
		{1, 0, 0},
		{-1, -1, 0},
		{0, -1, 0},
		{1, -1, 0},
		// Floor
		{-1, 1, -1},
		{0, 1, -1},
		{1, 1, -1},
		{-1, 0, -1},
		{0, 0, -1},
		{1, 0, -1},
		{-1, -1, -1},
		{0, -1, -1},
		{1, -1, -1},
	};

	for (auto Neighbour : Neighbours)
	{
		const auto Cell = Coordinate + Neighbour;
		if (!IsValidCoordinate(Cell))
		{
			WallCount++;
		}
		else
		{
			const int Index = Cell.Z * MapSize.Y * MapSize.X + Cell.Y * MapSize.X + Cell.X;
			if (Map[Index] == 1)
			{
				WallCount++;
			}
		}
	} 
	
	return WallCount;
}

bool AGrid3D::IsValidCoordinate(const FVector& Coordinate) const
{
	return Coordinate.X >= 0 && Coordinate.X < MapSize.X
		&& Coordinate.Y >= 0 && Coordinate.Y < MapSize.Y
		&& Coordinate.Z >= 0 && Coordinate.Z < MapSize.Z;
}

void AGrid3D::Step()
{
	TArray<int> NewMap =  Map;
	for (int z = 0; z < MapSize.Z; z++)
	{
		for (int y = 0; y < MapSize.Y; y++)
		{
			for (int x = 0; x < MapSize.X; x++)
			{
				const int Index = z * MapSize.Y * MapSize.X + y * MapSize.X + x;
				const int Count = CountCellWalls(FVector(x, y, z));
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
	}

	Map = NewMap;
}
