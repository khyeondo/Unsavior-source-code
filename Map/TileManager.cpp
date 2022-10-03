// Fill out your copyright notice in the Description page of Project Settings.


#include "TileManager.h"
#include "Tile.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

// Sets default values
ATileManager::ATileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PrePlayerTilePos = FIntVector::ZeroValue;

	SeedX = FMath::Rand();
	SeedY = FMath::Rand();

	UE_LOG(LogTemp, Log, TEXT("SeedX %d"), SeedX);
	UE_LOG(LogTemp, Log, TEXT("SeedY %d"), SeedY);
	//SeedX = 10000;
}

// Called when the game starts or when spawned
void ATileManager::BeginPlay()
{
	Super::BeginPlay();
	for (int32 i = -1; i < 2; i++)
	{
		for (int32 j = -1; j < 2; j++)
		{
			//ATile* tile = GetWorld()->SpawnActor<ATile>(
			//	FindTileTypeFromPos(FIntVector(i, j, 0)).Get(),
			//	ConvertTilePosToWorldPos(FIntVector(i, j, 0)),
			//	FRotator::ZeroRotator
			//	);			
			ATile* tile = GetWorld()->SpawnActorDeferred<ATile>(
				FindTileTypeFromPos(FIntVector(i, j, 0)).Get(),
				FTransform(ConvertTilePosToWorldPos(FIntVector(i, j, 0)))
				); 			

			SpawnedTile.Add(tile);
			tile->TileManager = this;
			tile->TilePos = FIntVector(i, j, 0);
			
			UGameplayStatics::FinishSpawningActor(
				tile,
				FTransform(ConvertTilePosToWorldPos(FIntVector(i, j, 0)))
			);

			if(i != 0 || j != 0)
				tile->OnPostInitValue();
			
			/*
			* y+
			* ↑
			* |258
			* |147
			* |036
			* -------> x+
			*/
		}
	}
}

void ATileManager::CustomTick(float DeltaTime)
{
	CurPlayerTilePos = FindPlayerTilePos();
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (IsValid(Player))
	{
		if (PrePlayerTilePos != CurPlayerTilePos)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("ATileManager::CustomTick: PrePlayerTilePos != CurPlayerTilePos"));
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("%d"), Player->GetIsInBossRoom()));
			if (Player->GetIsInBossRoom() == false)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("ATileManager::CustomTick: GetIsInBossRoom == false"));
				OnMoveToOtherTile();
				PrePlayerTilePos = CurPlayerTilePos;
			}
		}
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, FString::Printf(TEXT("ATileManager::CustomTick: %d,%d"),
			//CurPlayerTilePos.X,
			//CurPlayerTilePos.Y
		//));
	}
}

// Called every frame
//void ATileManager::Tick(float DeltaTime)
//{
	//Super::Tick(DeltaTime);
	//
	//CurPlayerTilePos = FindPlayerTilePos();
	//if (PrePlayerTilePos != CurPlayerTilePos)
	//{
	//	if (APlayerCharacter::GetInstance()->GetIsInBossRoom() == false) {
	//		OnMoveToOtherTile();	
	//		PrePlayerTilePos = CurPlayerTilePos;
	//	}
	//}

	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, FString::Printf(TEXT("%d,%d"),
	//	CurPlayerTilePos.X,
	//	CurPlayerTilePos.Y
	//));
	//
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, FString::Printf(TEXT("%f,%f"),
	//	ConvertTilePosToWorldPos(CurPlayerTilePos).X,
	//	ConvertTilePosToWorldPos(CurPlayerTilePos).Y
	//));

	
//}

void ATileManager::OnMoveToOtherTile()
{

	ATile* DestroyedTile[3];
	int DestroyedTileIdx = 0;

	NavMeshVolum->SetActorLocation(FVector(CurPlayerTilePos.X * TileSize, CurPlayerTilePos.Y * TileSize, 0.f));
	FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())->OnNavigationBoundsUpdated(NavMeshVolum);

	if (CurPlayerTilePos.X > PrePlayerTilePos.X)
	{		
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("OnMove"));
		for (size_t i = 0; i < SpawnedTile.Num(); i++)
		{
			if (SpawnedTile[i]->TilePos.X == (CurPlayerTilePos.X - 2))
			{
				SpawnedTile[i]->Destroy();
				DestroyedTile[DestroyedTileIdx] = SpawnedTile[i];
				DestroyedTileIdx++;
			}
		}
		for (int i = CurPlayerTilePos.Y-1; i < CurPlayerTilePos.Y + 2; i++)
		{
			ATile* tile = GetWorld()->SpawnActorDeferred<ATile>(
				FindTileTypeFromPos(FIntVector(CurPlayerTilePos.X + 1, i, 0)).Get(),
				FTransform(ConvertTilePosToWorldPos(FIntVector(CurPlayerTilePos.X + 1, i, 0))),
				nullptr, nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);

			SpawnedTile.Add(tile);
			tile->TileManager = this;
			tile->TilePos = FIntVector(CurPlayerTilePos.X + 1, i, 0);

			UGameplayStatics::FinishSpawningActor(
				tile,
				FTransform(ConvertTilePosToWorldPos(FIntVector(CurPlayerTilePos.X + 1, i, 0)))
			);
			tile->OnPostInitValue();

			/*
			ATile* tile = GetWorld()->SpawnActor<ATile>(
				FindTileTypeFromPos(FIntVector(CurPlayerTilePos.X + 1, i, 0)).Get(),
				ConvertTilePosToWorldPos(FIntVector(CurPlayerTilePos.X + 1, i, 0)),
				FRotator::ZeroRotator
				);

			SpawnedTile.Add(tile);
			tile->TileManager = this;
			tile->TilePos = FIntVector(CurPlayerTilePos.X + 1, i, 0);
			tile->OnPostInitValue();
			*/
		}
	}
	else if (CurPlayerTilePos.X < PrePlayerTilePos.X)
	{
		for (size_t i = 0; i < SpawnedTile.Num(); i++)
		{			
			if (SpawnedTile[i]->TilePos.X == (CurPlayerTilePos.X + 2))
			{
				SpawnedTile[i]->Destroy();
				DestroyedTile[DestroyedTileIdx] = SpawnedTile[i];
				DestroyedTileIdx++;
			}
		}
		for (int i = CurPlayerTilePos.Y - 1; i < CurPlayerTilePos.Y + 2; i++)
		{
			ATile* tile = GetWorld()->SpawnActorDeferred<ATile>(
				FindTileTypeFromPos(FIntVector(CurPlayerTilePos.X - 1, i, 0)).Get(),
				FTransform(ConvertTilePosToWorldPos(FIntVector(CurPlayerTilePos.X - 1, i, 0))),
				nullptr, nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);

			SpawnedTile.Add(tile);
			tile->TileManager = this;
			tile->TilePos = FIntVector(CurPlayerTilePos.X - 1, i, 0);

			UGameplayStatics::FinishSpawningActor(
				tile,
				FTransform(ConvertTilePosToWorldPos(FIntVector(CurPlayerTilePos.X - 1, i, 0)))
			);
			tile->OnPostInitValue();

			/*
			ATile* tile = GetWorld()->SpawnActor<ATile>(
				FindTileTypeFromPos(FIntVector(CurPlayerTilePos.X - 1, i, 0)).Get(),
				ConvertTilePosToWorldPos(FIntVector(CurPlayerTilePos.X - 1, i, 0)),
				FRotator::ZeroRotator
				);

			SpawnedTile.Add(tile);
			tile->TileManager = this;
			tile->TilePos = FIntVector(CurPlayerTilePos.X - 1, i, 0);
			tile->OnPostInitValue();
			*/
		}
	}
	else if (CurPlayerTilePos.Y > PrePlayerTilePos.Y)
	{
		for (size_t i = 0; i < SpawnedTile.Num(); i++)
		{
			if (SpawnedTile[i]->TilePos.Y == (CurPlayerTilePos.Y - 2))
			{
				SpawnedTile[i]->Destroy();
				DestroyedTile[DestroyedTileIdx] = SpawnedTile[i];
				DestroyedTileIdx++;
			}
		}
		for (int i = CurPlayerTilePos.X - 1; i < CurPlayerTilePos.X + 2; i++)
		{
			ATile* tile = GetWorld()->SpawnActorDeferred<ATile>(
				FindTileTypeFromPos(FIntVector(i, CurPlayerTilePos.Y + 1, 0)).Get(),
				FTransform(ConvertTilePosToWorldPos(FIntVector(i, CurPlayerTilePos.Y + 1, 0))),
				nullptr,nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);

			SpawnedTile.Add(tile);
			tile->TileManager = this;
			tile->TilePos = FIntVector(i, CurPlayerTilePos.Y + 1, 0);

			UGameplayStatics::FinishSpawningActor(
				tile,
				FTransform(ConvertTilePosToWorldPos(FIntVector(i, CurPlayerTilePos.Y + 1, 0)))
			);
			tile->OnPostInitValue();

			/*
			ATile* tile = GetWorld()->SpawnActor<ATile>(
				FindTileTypeFromPos(FIntVector(i, CurPlayerTilePos.Y + 1, 0)).Get(),
				ConvertTilePosToWorldPos(FIntVector(i, CurPlayerTilePos.Y + 1, 0)),
				FRotator::ZeroRotator
				);

			SpawnedTile.Add(tile);
			tile->TileManager = this;
			tile->TilePos = FIntVector(i, CurPlayerTilePos.Y + 1, 0);
			tile->OnPostInitValue();
			*/
		}
	}
	else if (CurPlayerTilePos.Y < PrePlayerTilePos.Y)
	{
		for (size_t i = 0; i < SpawnedTile.Num(); i++)
		{
			if (SpawnedTile[i]->TilePos.Y == (CurPlayerTilePos.Y + 2))
			{
				SpawnedTile[i]->Destroy();
				DestroyedTile[DestroyedTileIdx] = SpawnedTile[i];
				DestroyedTileIdx++;
			}
		}
		for (int i = CurPlayerTilePos.X - 1; i < CurPlayerTilePos.X + 2; i++)
		{
			ATile* tile = GetWorld()->SpawnActorDeferred<ATile>(
				FindTileTypeFromPos(FIntVector(i, CurPlayerTilePos.Y - 1, 0)).Get(),
				FTransform(ConvertTilePosToWorldPos(FIntVector(i, CurPlayerTilePos.Y - 1, 0))),
				nullptr, nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);

			SpawnedTile.Add(tile);
			tile->TileManager = this;
			tile->TilePos = FIntVector(i, CurPlayerTilePos.Y - 1, 0);

			UGameplayStatics::FinishSpawningActor(
				tile,
				FTransform(ConvertTilePosToWorldPos(FIntVector(i, CurPlayerTilePos.Y - 1, 0)))
			);
			tile->OnPostInitValue();

			/*
			ATile* tile = GetWorld()->SpawnActor<ATile>(
				FindTileTypeFromPos(FIntVector(i, CurPlayerTilePos.Y - 1, 0)).Get(),
				ConvertTilePosToWorldPos(FIntVector(i, CurPlayerTilePos.Y - 1, 0)),
				FRotator::ZeroRotator
				);

			SpawnedTile.Add(tile);
			tile->TileManager = this;
			tile->TilePos = FIntVector(i, CurPlayerTilePos.Y - 1,0);
			tile->OnPostInitValue();
			*/
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("TileManager::OnMoveToOtherTile : Tile is not Spawned"));
	}

	for (size_t i = 0; i < 3; i++)
	{
		DestroyedTile[i]->OnPreDestroy();
		DestroyedTile[i]->Destroy();
		SpawnedTile.Remove(DestroyedTile[i]);
	}

	for (ATile* tile : SpawnedTile)
	{
		if (tile->TilePos == CurPlayerTilePos)
		{
			tile->OnPlayerOnTile();
		}
	}
}

TSubclassOf<class AActor> ATileManager::FindTileTypeFromPos(FIntVector TilePos)
{
	TSubclassOf<AActor> tileType;
	if (CheckBossTileTypeFromPos(TilePos, tileType))
	{
		return tileType;
	}

	int32 X = TilePos.X;
	int32 Y = TilePos.Y;

	FRandomStream stream;

	stream.Initialize(GetTypeHash(FVector(X, Y, SeedY)));
	float rand;

	if (X % 2 == 0 && Y % 2 == 0)
	{
		return Stage[CurStage].CorridorType[stream.RandRange(0, Stage[CurStage].CorridorType.Num() - 1)];
		//return CorridorType;
	}
	else if ((FMath::Abs(X%2) == 1 && (FMath::Abs(Y % 2) == 1)))
	{
		//if(GetTileTypeFromPos(FIntVector(TilePos.X-1,TilePos.Y,0)) &&
		//	GetTileTypeFromPos(FIntVector(TilePos.X + 1, TilePos.Y, 0)) &&
		//	GetTileTypeFromPos(FIntVector(TilePos.X, TilePos.Y - 1, 0)) &&
		//	GetTileTypeFromPos(FIntVector(TilePos.X, TilePos.Y + 1, 0)))

		rand = stream.FRandRange(0.f, 1.0f);
		if (rand < Stage[CurStage].HiddenTileProbability)
		{
			stream.Initialize(GetTypeHash(FVector(X, Y, SeedX)));
			rand = stream.FRandRange(0.f, 1.f);

			float randomRange = 1.f / Stage[CurStage].HiddenTileType.Num();
			for (auto type : Stage[CurStage].HiddenTileType)
			{
				if (rand < randomRange)
				{
					return type;
				}
				randomRange += 1.f / Stage[CurStage].RoomType.Num();
			}

			return *(Stage[CurStage].HiddenTileType).begin();
		}

		stream.Initialize(GetTypeHash(FVector(X, Y, SeedX)));
		rand = stream.FRandRange(0.f, 1.f);

		float randomRange = 1.f / Stage[CurStage].RoomType.Num();
		for (auto type : Stage[CurStage].RoomType)
		{
			if (rand < randomRange)
			{
				return type;
			}
			randomRange += 1.f / Stage[CurStage].RoomType.Num();
		}

		return Stage[CurStage].RoomType[0];
	}

	//uint32 seed = GetTypeHash(FVector(X, Y, SeedX));
	//FRandomStream stream(seed);	
	//float rand = stream.FRandRange(0.f, 1.0f);

	stream.Initialize(SeedX + X + 2 * Y);
	stream.Initialize(stream.GetUnsignedInt());
	rand = stream.FRandRange(0.f, 0.5f);
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("%f"), rand));
	stream.Initialize(SeedY + 2 * X + Y);
	stream.Initialize(stream.GetUnsignedInt());
	rand += stream.FRandRange(0.f, 0.5f);

	if (rand > 0.4f) {
		return Stage[CurStage].CorridorType[stream.RandRange(0, Stage[CurStage].CorridorType.Num() - 1)];
		//return CorridorType;
	}
	

	stream.Initialize(GetTypeHash(FVector(X, Y, SeedX)));
	rand = stream.FRandRange(0.f, 1.f);

	float randomRange = 1.f/ Stage[CurStage].RoomType.Num();
	for (auto type : Stage[CurStage].RoomType)
	{
		if (rand < randomRange)
		{
			return type;
		}
		randomRange += 1.f / Stage[CurStage].RoomType.Num();
	}

	return *(Stage[CurStage].RoomType).end();

}

bool ATileManager::CheckBossTileTypeFromPos(FIntVector TilePos, TSubclassOf<class AActor>& OutType)
{
	FRandomStream stream;

	int interval = 6; //무조건 5보다 커야함 

	//if ((TilePos.X <= 2 && TilePos.X >= -2) && TilePos.Y <= 2 && TilePos.Y >= -2)
	//	return false;

	if ((int)(TilePos.X / interval) == 0 && ((int)(TilePos.Y / interval) == 0))
	{		
		return false;
	}

	stream.Initialize(GetTypeHash(FIntVector((int)(TilePos.X / interval), (int)(TilePos.Y / interval), SeedX)));
	//int dir = stream.RandHelper(5);	

	if (stream.FRandRange(0.f, 1.0f) <= Stage[CurStage].BossRoomProbability)
	{
		FIntVector CenterPos;
		CenterPos.X = stream.RandRange(2, interval - 2);
		CenterPos.Y = stream.RandRange(2, interval - 2);
		
		if (TilePos.X < 0)
			CenterPos.X *= -1;
		if (TilePos.Y < 0)
			CenterPos.Y *= -1;

		CenterPos.X += (int)(TilePos.X / interval) * interval;
		CenterPos.Y += (int)(TilePos.Y / interval) * interval;

		if (TilePos.X == CenterPos.X - 1 && TilePos.Y == CenterPos.Y- 1)
		{
			OutType = Stage[CurStage].BossRoomWall.LeftTop;
			return true;
		}
		else if (TilePos.X == CenterPos.X && TilePos.Y == CenterPos.Y - 1)
		{
			OutType = Stage[CurStage].BossRoomWall.Top;
			return true;
		}
		else if (TilePos.X == CenterPos.X + 1 && TilePos.Y == CenterPos.Y - 1)
		{
			OutType = Stage[CurStage].BossRoomWall.RightTop;
			return true;
		}
		else if (TilePos.X == CenterPos.X + 1 && TilePos.Y == CenterPos.Y)
		{
			OutType = Stage[CurStage].BossRoomWall.Right;
			return true;
		}
		else if (TilePos.X == CenterPos.X + 1 && TilePos.Y == CenterPos.Y + 1)
		{
			OutType = Stage[CurStage].BossRoomWall.RightBottom;
			return true;
		}
		else if (TilePos.X == CenterPos.X && TilePos.Y == CenterPos.Y + 1)
		{
			OutType = Stage[CurStage].BossRoomWall.Bottom;
			return true;
		}
		else if (TilePos.X == CenterPos.X - 1&& TilePos.Y == CenterPos.Y + 1)
		{
			OutType = Stage[CurStage].BossRoomWall.LeftBottom;
			return true;
		}
		else if (TilePos.X == CenterPos.X - 1 && TilePos.Y == CenterPos.Y)
		{
			OutType = Stage[CurStage].BossRoomWall.Left;
			return true;
		}
		else if(TilePos.X == CenterPos.X && TilePos.Y == CenterPos.Y)
		{
			OutType = Stage[CurStage].BossRoomType;
			return true;
		}
		
		/*
		if (TilePos.X % interval == 0 && TilePos.Y % interval == 0) {
			OutType = Stage[CurStage].BossRoomWall.LeftTop;
			return true;
		}
		else if (TilePos.X % interval == 1 && TilePos.Y % interval == 0) {
			OutType = Stage[CurStage].BossRoomWall.Top;
			return true;
		}
		else if (TilePos.X % interval == 2 && TilePos.Y % interval == 0) {
			OutType = Stage[CurStage].BossRoomWall.RightTop;
			return true;
		}
		else if (TilePos.X % interval == 2 && TilePos.Y % interval == 1) {
			OutType = Stage[CurStage].BossRoomWall.Right;
			return true;
		}
		else if (TilePos.X % interval == 2 && TilePos.Y % interval == 2) {
			OutType = Stage[CurStage].BossRoomWall.RightBottom;
			return true;
		}
		else if (TilePos.X % interval == 1 && TilePos.Y % interval == 2) {
			OutType = Stage[CurStage].BossRoomWall.Bottom;
			return true;
		}
		else if (TilePos.X % interval == 0 && TilePos.Y % interval == 2) {
			OutType = Stage[CurStage].BossRoomWall.LeftBottom;
			return true;
		}
		else if (TilePos.X % interval == 0 && TilePos.Y % interval == 1) {
			OutType = Stage[CurStage].BossRoomWall.Left;
			return true;
		}
		else if (TilePos.X % interval == 1 && TilePos.Y % interval == 1){
			OutType = Stage[CurStage].BossRoomType;			
			return true;
		}
		*/
	}
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("bb"));

	return false;
}

FVector ATileManager::ConvertTilePosToWorldPos(FIntVector TilePos)
{
	return FVector(TilePos.X * TileSize, TilePos.Y * TileSize , 0.f);
}

FIntVector ATileManager::ConvertWorldPosToTilePos(FVector WorldPos)
{
	FIntVector tilePos = 
		FIntVector(
			(int)FMath::Floor((WorldPos.X + TileSize / 2.f) / TileSize),
			(int)FMath::Floor((WorldPos.Y + TileSize / 2.f) / TileSize),
			0	
		);
	
	//if (WorldPos.X < -1.f * TileSize / 2.f)
	//{
	//	tilePos.X -= 1.f;
	//}
	//
	//if (WorldPos.Y < -1.f * TileSize / 2.f)
	//{
	//	tilePos.Y -= 1.f;
	//}

	return tilePos;
}

float ATileManager::GetTileSize()
{
	return TileSize;
}

bool ATileManager::GetIsClear(FIntVector TilePos)
{
	bool* isClear = IsClear.Find(TilePos);

	if (IsClear.Find(TilePos) == nullptr)
	{
		return false;
	}
	return *isClear;
}

void ATileManager::SetIsClear(FIntVector TilePos, bool isClear)
{	
	IsClear.Add(TilePos, isClear);
}

void ATileManager::AddStage()
{	
	if (CurStage < Stage.Num()-1  )
	{		
		CurStage++;
		OnAddStage();
	}
}

int ATileManager::GetStage()
{
	return CurStage;
}

void ATileManager::OnAddStage_Implementation()
{
}

FIntVector ATileManager::FindPlayerTilePos()
{
	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	//FIntVector PlayerTilePos = 

	//PlayerTilePos.X = (int)((PlayerLocation.X + TileSize / 2.f) / TileSize);
	//if (PlayerLocation.X < -1.f * TileSize / 2.f)
	//{
	//	PlayerTilePos.X -= 1.f;
	//}
	//
	//PlayerTilePos.Y = (int)((PlayerLocation.Y + TileSize / 2.f) / TileSize);
	//if (PlayerLocation.Y < -1.f * TileSize / 2.f)
	//{
	//	PlayerTilePos.Y -= 1.f;
	//}

	return ConvertWorldPosToTilePos(PlayerLocation);
}

