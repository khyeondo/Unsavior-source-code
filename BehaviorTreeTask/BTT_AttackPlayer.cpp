// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_AttackPlayer.h"
#include "AIController.h"
#include "Monster.h"

UBTT_AttackPlayer::UBTT_AttackPlayer()
{
    NodeName = TEXT("AttackPlayer");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTT_AttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    OwnerComp.GetAIOwner()->GetPawn<AMonster>()->Attack();

    return EBTNodeResult::InProgress;
}

void UBTT_AttackPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    //OwnerComp.GetAIOwner()->GetPawn<AMonster>()->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
    if (OwnerComp.GetAIOwner()->GetPawn<AMonster>()->GetCurrentMontage() == nullptr)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }    
}
