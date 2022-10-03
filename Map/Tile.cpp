// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "TileManager.h"
#include "Monster.h"
#include "HitBoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATile::ATile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	MonsterSpawnLoop.SetLength(TotalMonsterNum);

	SpawnRandomTrap();
}

void ATile::OnPlayerOnTile_Implementation()
{
}

void ATile::SpawnRandomTrap()
{
	if (TilePos != FIntVector(0, 0, 0))
	{
		if (SpawnTrapType.Num() != 0)
		{
			if (SpawnTrapsInThisTile)
			{
				if (FMath::RandRange(1.f, 100.f) <= ChanceOfTrap)
				{
					size_t TrapIndex = FMath::RandRange(0, SpawnTrapType.Num() - 1);

					const FVector TrapLocation = FVector(
						GetActorLocation().X,
						GetActorLocation().Y,
						100.f);


					if (IsValid(SpawnTrapType[TrapIndex]))
					{
						SpawnedTrap = GetWorld()->SpawnActor<AActor>(
							SpawnTrapType[TrapIndex],
							TrapLocation,
							FRotator(0.f, 0.f, 0.f)
							);
					}
				}
			}
		}
	}
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (StageSpawnMonsterId.Num() != 0)
	{
		int StageIdx = FMath::Min(TileManager->GetStage(), StageSpawnMonsterId.Num() - 1);
		if (IsInit && StageSpawnMonsterId[StageIdx].SpawnMonsterId.Num() != 0)
		{
			MonsterSpawnLoop([this, StageIdx](int i) {
				FBox Box = FBox(FVector(-1.f * (TileManager->GetTileSize() / 2.f - 30.f), -1.f * (TileManager->GetTileSize() / 2.f - 30.f), 0.0f),
					FVector((TileManager->GetTileSize() / 2.f - 30.f), (TileManager->GetTileSize() / 2.f - 30.f), 0.0f));
				FVector SpawnPos = FMath::RandPointInBox(Box) + GetActorLocation() + FVector(0, 0, 100.f);

				//AMonster::SpawnUsingDataTable(this, SpawnMonsterId[i % SpawnMonsterId.Num()], FTransform(SpawnPos));


				AMonster::SpawnUsingDataTable(this,
					StageSpawnMonsterId[StageIdx].SpawnMonsterId[i % StageSpawnMonsterId[StageIdx].SpawnMonsterId.Num()],
					FTransform(SpawnPos));

				//AMonster::SpawnByType(this, SpawnMonsterType[i% SpawnMonsterType.Num()], FName(""), 300, 300, 200, 10, 0, 100, 0, 0, 0, 0, EDebuff::None, FTransform(SpawnPos));
				});
		}
	}

}

void ATile::OnPreDestroy()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> Types;
	//if(TargetType == TEXT("Monster"))
	Types.Add(UEngineTypes::ConvertToObjectType(MonsterCollisionChannel));
	//else if(TargetType == TEXT("Player"))

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::BoxOverlapActors(
		this,
		GetActorLocation(),
		FVector(TileManager->GetTileSize() / 2.f, TileManager->GetTileSize() / 2.f, 100.f),
		Types,
		TSubclassOf<AMonster>(),
		TArray<AActor*>(),
		OutActors
	);

	for (auto Monster : OutActors)
	{
		Monster->Destroy();
	}
}

void ATile::BeginDestroy()
{
	Super::BeginDestroy();

}

void ATile::OnPostInitValue()
{
	if (Wall_XPlus != nullptr)
	{		
		UClass* TileClass = *TileManager->FindTileTypeFromPos(TilePos + FIntVector(1, 0, 0));

		if (GetClass() == TileClass ||
			TileClass->GetName() == TEXT("BP_EliteMonsterTile_C"))
		{			
			TArray<USceneComponent*> ChildrenComponent;
			Wall_XPlus->GetChildrenComponents(true, ChildrenComponent);

			for (auto child : ChildrenComponent)
			{
				child->DestroyComponent();
			}

			Wall_XPlus->DestroyComponent();
		}
	}
	if (Wall_XMinus != nullptr)
	{
		UClass * TileClass = *TileManager->FindTileTypeFromPos(TilePos + FIntVector(-1, 0, 0));
		if (GetClass() == TileClass ||
			TileClass->GetName() == TEXT("BP_EliteMonsterTile_C"))
		{
			TArray<USceneComponent*> ChildrenComponent;
			Wall_XMinus->GetChildrenComponents(true, ChildrenComponent);

			for (auto child : ChildrenComponent)
			{
				child->DestroyComponent();
			}

			Wall_XMinus->DestroyComponent();
		}
	}
	if (Wall_YMinus != nullptr)
	{
		UClass* TileClass = *TileManager->FindTileTypeFromPos(TilePos + FIntVector(0, -1, 0));
		if (GetClass() == TileClass ||
			TileClass->GetName() == TEXT("BP_EliteMonsterTile_C"))
		{
			TArray<USceneComponent*> ChildrenComponent;
			Wall_YMinus->GetChildrenComponents(true, ChildrenComponent);

			for (auto child : ChildrenComponent)
			{
				child->DestroyComponent();
			}
			Wall_YMinus->DestroyComponent();
		}
	}
	if (Wall_YPlus != nullptr)
	{
		UClass* TileClass = *TileManager->FindTileTypeFromPos(TilePos + FIntVector(0, 1, 0));
		if (GetClass() == TileClass ||
			TileClass->GetName() == TEXT("BP_EliteMonsterTile_C"))
		{
			TArray<USceneComponent*> ChildrenComponent;
			Wall_YPlus->GetChildrenComponents(true, ChildrenComponent);

			for (auto child : ChildrenComponent)
			{
				child->DestroyComponent();
			}
			Wall_YPlus->DestroyComponent();
		}
	}

	IsInit = true;
}
