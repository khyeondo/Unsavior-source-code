    // Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_RunawayFromPlayer.h"
#include "MonsterController.h"
#include "PlayerCharacter.h"
#include "Monster.h"

UBTT_RunawayFromPlayer::UBTT_RunawayFromPlayer()
{
    NodeName = TEXT("RunawayFromPlayer");
    bNotifyTick = true;
    bNotifyTaskFinished = true;

    Timer = 0.f;
}

EBTNodeResult::Type UBTT_RunawayFromPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    Timer = 0.f;

    OwnerComp.GetAIOwner()->GetPawn<AMonster>()->SetIsWalk(IsWalk);

    return EBTNodeResult::InProgress;
}


void UBTT_RunawayFromPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    Monster_Owner = OwnerComp.GetAIOwner()->GetPawn<AMonster>();
    AActor* Player = Monster_Owner->GetPlayer();

    FVector dir = Monster_Owner->GetActorLocation() - Player->GetActorLocation();
    dir.Normalize();

    Monster_Owner->TurnToMoveDirection(TurnSpeed);
    Monster_Owner->MoveToLocation(Monster_Owner->GetActorLocation() + dir * 500.f);

    if (TimeLimit != -1.f)
    {
        Timer += DeltaSeconds;
        if(Timer >= TimeLimit)
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

void UBTT_RunawayFromPlayer::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    OwnerComp.GetAIOwner()->StopMovement();
}
