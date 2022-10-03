// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MoveToPlayer.h"
#include "AIController.h"
#include "MonsterController.h"
#include "Monster.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_MoveToPlayer::UBTT_MoveToPlayer()
{
	NodeName = TEXT("MoveToPlayer");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTT_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComp.GetAIOwner()->GetPawn<AMonster>()->SetIsWalk(IsWalk);

	return EBTNodeResult::InProgress;
}

void UBTT_MoveToPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	Monster_Owner = OwnerComp.GetAIOwner()->GetPawn<AMonster>();

	Monster_Owner->TurnToMoveDirection(TurnSpeed);
	Monster_Owner->MoveToPlayer();

	//if (Monster_Owner->DistToPlayer() < Monster_Owner->GetAttackRange())
	//{
	//	OwnerComp.GetAIOwner()->StopMovement();
	//	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	//}
}

void UBTT_MoveToPlayer::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	OwnerComp.GetAIOwner()->StopMovement();
}
