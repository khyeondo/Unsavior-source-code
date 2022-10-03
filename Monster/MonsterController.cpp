// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster.h"

const FName AMonsterController::Key_TaskOwner(TEXT("TaskOwner"));

AMonsterController::AMonsterController()
{
	/*static ConstructorHelpers::FObjectFinder<UBlackboardData> BB_Object(TEXT("BlackboardData'/Game/Object/Monster/AI/BB_Monster.BB_Monster'"));
	if (BB_Object.Succeeded())
	{
		UE_LOG(LogTemp, Log, TEXT("Blackboard Load Succeeded"));
		BB_Monster = BB_Object.Object;
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT_Object(TEXT("BehaviorTree'/Game/Object/Monster/AI/BT_Monster.BT_Monster'"));
	if (BT_Object.Succeeded())
	{
		UE_LOG(LogTemp, Log, TEXT("Behavior Tree Load Succeeded"));
		BT_Monster = BT_Object.Object;
	}*/
}

bool AMonsterController::GetIsPathFollowing()
{
	return IsPathFollowing;
}

void AMonsterController::SetActive(bool b)
{
	SetActorHiddenInGame(!b);
	SetActorEnableCollision(b);
	SetActorTickEnabled(b);
}

FPathFollowingRequestResult AMonsterController::MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath)
{
	IsPathFollowing = true;
	return Super::MoveTo(MoveRequest,OutPath);
}

void AMonsterController::BeginPlay()
{
	Super::BeginPlay();

	TaskOwnedMonster = GetPawn<AMonster>();
}

void AMonsterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AMonster* MonsterBot = Cast<AMonster>(InPawn);

	RunAI();
}

void AMonsterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	IsPathFollowing = false;
}

void AMonsterController::RunAI()
{
	if (UseBlackboard(BB_Monster, Blackboard))
	{
		RunBehaviorTree(BT_Monster);
	}
}