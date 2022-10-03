// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MoveToPlayerDuringTime.h"
#include "MonsterController.h"
#include "Monster.h"

UBTT_MoveToPlayerDuringTime::UBTT_MoveToPlayerDuringTime()
{
	NodeName = TEXT("MoveToPlayerDuringTime");
	bNotifyTick = true;
	bNotifyTaskFinished = true;

	Timer = 0.f;
}


EBTNodeResult::Type UBTT_MoveToPlayerDuringTime::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComp.GetAIOwner()->GetPawn<AMonster>()->SetIsWalk(IsWalk);

	return EBTNodeResult::InProgress;
}

void UBTT_MoveToPlayerDuringTime::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	Monster = OwnerComp.GetAIOwner()->GetPawn<AMonster>();

	Timer += DeltaSeconds;

	if (!IsValid(Monster))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("BTT_MoveToPlayerDuringTime::TickTask(): Monster is not valid"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	if (Timer <= Time)
	{
		if (Monster->DistToPlayer() > Monster->GetAttackRange())
		{
			Monster->TurnToMoveDirection(TurnSpeed);
			Monster->MoveToPlayer();
		}
		else
		{
			OwnerComp.GetAIOwner()->StopMovement();
			Monster->TurnToPlayer(10.f);
		}


	}
	else
	{
		Timer -= Time;
		OwnerComp.GetAIOwner()->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

