// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "LoopPerFrame.h"
#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "DefaultLevelScriptActor.generated.h"

//USTRUCT()
//struct FRatio
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditAnywhere)
//	FString id;
//	UPROPERTY(EditAnywhere)
//	uint8 ratio;
//};
//
//USTRUCT()
//struct FSpawnRange
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditAnywhere)
//	TArray<FString> theme;
//	UPROPERTY(EditAnywhere)
//	uint32 level;
//};

//USTRUCT()
//struct FTileColumn
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditAnywhere)
//	TArray<class ATile*> Column;
//};
//
//USTRUCT()
//struct FMonsterTheme
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditAnywhere)
//	uint8 TotalMonsterNum;
//	UPROPERTY(EditAnywhere)
//	TArray<FRatio> Ratio;
//};

UCLASS()
class TEAMPROJECT_API ADefaultLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

protected:
	//UPROPERTY(EditAnywhere)
	class UDataTable* MonsterDataTable;

private:
	TMap<FString, TSubclassOf<class AMonster>> MonsterClasses;

#pragma region ObjectPooling	
	int CurSpawnLevelIdx = 0;


#pragma endregion

	uint32 PlayerLevel = 1;

protected:
	UPROPERTY(BlueprintReadOnly)
		FVector2D PlayerTilePos = FVector2D(0, 0);

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	ADefaultLevelScriptActor();

	//void OnPlayerLevelUp(uint32 level);

	void AddMonsterClass(FString Id);
	TSubclassOf<AMonster> GetMonsterClassById(FString Id);

	struct FMonsterData* GetMonsterData(FName Id);
};

