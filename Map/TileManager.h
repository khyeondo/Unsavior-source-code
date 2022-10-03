// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileManager.generated.h"

USTRUCT(BlueprintType)
struct TEAMPROJECT_API FBossRoomWall
{
	GENERATED_BODY()

		FBossRoomWall() {}

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ATile> LeftTop;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ATile> Top;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ATile> RightTop;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ATile> Right;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ATile> RightBottom;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ATile> Bottom;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ATile> LeftBottom;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ATile> Left;
};

USTRUCT(BlueprintType)
struct FStage
{
	GENERATED_BODY()

		FStage() {}

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<class ATile>> CorridorType;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ATile> BossRoomType;
	UPROPERTY(EditAnywhere)
		FBossRoomWall BossRoomWall;
	UPROPERTY(EditAnywhere)
		float BossRoomProbability = 0.1f;
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<class ATile>> RoomType;
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<class ATile>> HiddenTileType;
	UPROPERTY(EditAnywhere)
		float HiddenTileProbability = 0.1f;
};
UCLASS()
class TEAMPROJECT_API ATileManager : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
		float TileSize;

	UPROPERTY(EditAnywhere)
		TArray<FStage> Stage;

	UPROPERTY(EditAnywhere)
		class ANavMeshBoundsVolume* NavMeshVolum;
private:
	FIntVector CurPlayerTilePos;
	FIntVector PrePlayerTilePos;



	TArray<class ATile*> SpawnedTile;

	TMap<FIntVector, bool> IsClear;

	TMap<FIntVector, TSubclassOf<class ATile>> BossTile;

protected:
	UPROPERTY(BlueprintReadWrite)
		int32 SeedX;
	UPROPERTY(BlueprintReadWrite)
		int32 SeedY;

	UPROPERTY(BlueprintReadOnly)
		int CurStage = 0;

public:
	UPROPERTY(BlueprintReadWrite)
		int BossCount = 0;

public:
	// Sets default values for this actor's properties
	ATileManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	void CustomTick(float DeltaTime);
	void OnMoveToOtherTile();

	TSubclassOf<class AActor> FindTileTypeFromPos(FIntVector TilePos);
	bool CheckBossTileTypeFromPos(FIntVector TilePos, TSubclassOf<class AActor>& OutType);
	FIntVector FindPlayerTilePos();

	FVector ConvertTilePosToWorldPos(FIntVector TilePos);
	FIntVector ConvertWorldPosToTilePos(FVector WorldPos);

	float GetTileSize();

	UFUNCTION(BlueprintPure)
		bool GetIsClear(FIntVector TilePos);
	UFUNCTION(BlueprintCallable)
		void SetIsClear(FIntVector TilePos, bool isClear);
	UFUNCTION(BlueprintCallable)
		void AddStage();
	UFUNCTION(BlueprintPure)
		int GetStage();

	UFUNCTION(BlueprintNativeEvent)
		void OnAddStage();
	virtual void OnAddStage_Implementation();

};