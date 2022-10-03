// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ElitMonsterAttack.h"
#include "EliteMonster.h"
#include "AIController.h"

UBTT_ElitMonsterAttack::UBTT_ElitMonsterAttack()
{
	NodeName = TEXT("ElitMonsterAttack");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_ElitMonsterAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetAIOwner()->GetPawn<AEliteMonster>()->PlayAttack(AttackName);
	return EBTNodeResult::Type::InProgress;
}

void UBTT_ElitMonsterAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!OwnerComp.GetAIOwner()->GetPawn<AEliteMonster>()->IsPlayingAttack())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
