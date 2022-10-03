// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ElitMonsterSideStep.h"
#include "AIController.h"
#include "EliteMonster.h"

UBTT_ElitMonsterSideStep::UBTT_ElitMonsterSideStep()
{
	NodeName = TEXT("ElitMonsterSideStep");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTT_ElitMonsterSideStep::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEliteMonster* Monster = OwnerComp.GetAIOwner()->GetPawn<AEliteMonster>();
	if (IsValid(Monster))
	{
		Monster->SetIsWalk(IsWalk);
	}

	if (RandomDirection)
	{
		IsRight = FMath::RandBool();
	}

	return EBTNodeResult::Type::InProgress;
}

void UBTT_ElitMonsterSideStep::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AEliteMonster* Monster = OwnerComp.GetAIOwner()->GetPawn<AEliteMonster>();
	if (IsValid(Monster))
	{
		Monster->TurnToPlayer(TurnSpeed);
		Monster->SideStep(IsRight);
	}
}

void UBTT_ElitMonsterSideStep::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	OwnerComp.GetAIOwner()->StopMovement();
}
