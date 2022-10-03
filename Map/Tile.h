// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoopPerFrame.h"
#include "DefaultLevelScriptActor.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

USTRUCT(BlueprintType)
struct TEAMPROJECT_API FStageMonster
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FName> SpawnMonsterId;
};

UCLASS()
class TEAMPROJECT_API ATile : public AActor
{
	GENERATED_BODY()

	friend class ATileManager;

protected:	
	UPROPERTY(VisibleAnywhere, blueprintreadwrite, Category = Wall)
	class UStaticMeshComponent* Wall_XPlus = nullptr;
	UPROPERTY(VisibleAnywhere, blueprintreadwrite, Category = Wall)
	class UStaticMeshComponent* Wall_XMinus = nullptr;
	UPROPERTY(VisibleAnywhere, blueprintreadwrite, Category = Wall)
	class UStaticMeshComponent* Wall_YPlus = nullptr;
	UPROPERTY(VisibleAnywhere, blueprintreadwrite, Category = Wall)
	class UStaticMeshComponent* Wall_YMinus = nullptr;

	//UPROPERTY(EditAnywhere)
	//TArray<TSubclassOf<class AMonster>> SpawnMonsterType;
	UPROPERTY(EditAnywhere)
	TArray<FName> SpawnMonsterId;
	UPROPERTY(EditAnywhere)
	TArray<FStageMonster> StageSpawnMonsterId;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> SpawnTrapType;
	UPROPERTY(BlueprintReadOnly)
	AActor* SpawnedTrap;
	UPROPERTY(EditAnywhere)
	bool SpawnTrapsInThisTile = false;
	UPROPERTY(EditAnywhere)
	float ChanceOfTrap = 50.f;

	UPROPERTY(EditAnywhere)
	uint32 TotalMonsterNum;

	UPROPERTY(BlueprintReadOnly)
	class ATileManager* TileManager;
	UPROPERTY(BlueprintReadOnly)
	FIntVector TilePos;

private:
	bool IsInit = false;	
	
	LoopPerFrame MonsterSpawnLoop;

public:
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnPlayerOnTile();
	virtual void OnPlayerOnTile_Implementation();

	UFUNCTION()
	void SpawnRandomTrap();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void OnPreDestroy();
	virtual void BeginDestroy() override;	

	void OnPostInitValue();
};
