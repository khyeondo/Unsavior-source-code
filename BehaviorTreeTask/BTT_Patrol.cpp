// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Patrol.h"
#include "Monster.h"
#include "MonsterController.h"

UBTT_Patrol::UBTT_Patrol()
{
	NodeName = TEXT("Patrol");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}


EBTNodeResult::Type UBTT_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Monster = Cast<AMonster>(OwnerComp.GetAIOwner()->GetPawn());	

	Monster->SetIsWalk(IsWalk);

	if (!IsValid(Monster))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("BTT_Patrol::ExecuteTask(): Mosnter is not valid"));
		return EBTNodeResult::Type::Failed;
	}
	
	//Monster->AddStatusMulValue(EStatus::Speed, -0.4f);

	FVector unitVector = FMath::VRand();	
	unitVector *= FMath::FRandRange(MinDist, MaxDist);

	Monster->MoveToLocation(Monster->GetActorLocation() + unitVector);

	//OwnerComp.GetAIOwner()->MoveToLocation(Monster->GetActorLocation() + unitVector);

	return EBTNodeResult::Type::InProgress;
}

void UBTT_Patrol::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Monster = Cast<AMonster>(OwnerComp.GetAIOwner()->GetPawn());

	if (!IsValid(Monster))
	{
		return;
	}
	OwnerComp.GetAIOwner()->StopMovement();
	//Monster->AddStatusMulValue(EStatus::Speed, 0.4f);
}

void UBTT_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Monster = Cast<AMonster>(OwnerComp.GetAIOwner()->GetPawn());
	MonsterController = Cast<AMonsterController>(OwnerComp.GetAIOwner());

	if (!IsValid(Monster) || !IsValid(MonsterController))
	{
		return;
	}

	Monster->TurnToMoveDirection(TurnSpeed);
	if (MonsterController->GetIsPathFollowing() == false)
	{
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

