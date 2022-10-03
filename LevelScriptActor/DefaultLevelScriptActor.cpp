// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultLevelScriptActor.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Engine/DataTable.h"
#include "Tile.h"
#include "Monster.h"
#include "CustomDataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void ADefaultLevelScriptActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ADefaultLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
}

void ADefaultLevelScriptActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ADefaultLevelScriptActor::ADefaultLevelScriptActor()
{
	//AddMonsterClass(TEXT("TestMonsters"));
	//AddMonsterClass(TEXT("Skeleton"));
	//AddMonsterClass(TEXT("Archer"));
	//AddMonsterClass(TEXT("SkeletonWarlord"));

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Object/Monsters/DB/DB_MonsterDataTable.DB_MonsterDataTable'"));

	if (DataTable.Succeeded())
	{
		MonsterDataTable = DataTable.Object;

		for (auto id : MonsterDataTable->GetRowMap()) {
			AddMonsterClass(id.Key.ToString());
		}
	}
	else
	{
		UE_LOG(LogLoad, Error, TEXT("ADefaultLevelScriptActor::Constructor(): MonsterDataTable is not valid"));
	}
	//if (!IsValid(MonsterDataTable))
	//{
	//	UE_LOG(LogLoad, Error, TEXT("ADefaultLevelScriptActor::Constructor(): MonsterDataTable is not valid"));
	//	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("ADefaultLevelScriptActor::Constructor(): MonsterDataTable is not valid"));		
	//}
	//else
	//{
	//	for (auto id : MonsterDataTable->GetRowMap()) {
	//		AddMonsterClass(id.Key.ToString());
	//	}
	//}

}

TSubclassOf<AMonster> ADefaultLevelScriptActor::GetMonsterClassById(FString Id)
{
	if (MonsterClasses.Contains(Id))
		return MonsterClasses[Id];
	else
		return TSubclassOf<AMonster>();

	
}

void ADefaultLevelScriptActor::AddMonsterClass(FString Id)
{
	ConstructorHelpers::FClassFinder<AMonster> MonsterBPClass(*FString::Printf(TEXT("Blueprint'/Game/Object/Monsters/%s/BP_%s.BP_%s_C'"), *Id, *Id, *Id));
	MonsterClasses.Add(Id, MonsterBPClass.Class);
}

FMonsterData* ADefaultLevelScriptActor::GetMonsterData(FName Id)
{
	if (!IsValid(MonsterDataTable))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("ADefaultLevelScriptActor::GetMonsterData(): MonsterDataTable is not valid"));
		return nullptr;
	}
	return MonsterDataTable->FindRow<FMonsterData>(Id,TEXT(""));
}
