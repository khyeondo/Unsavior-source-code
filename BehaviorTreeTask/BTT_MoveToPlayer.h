// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MoveToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UBTT_MoveToPlayer : public UBTTaskNode
{
	GENERATED_BODY()

private:
	UPROPERTY()
		class AMonster* Monster_Owner;

	UPROPERTY(EditAnywhere)
		bool IsWalk = false;
	UPROPERTY(EditAnywhere)
		float TurnSpeed = 1.f;

protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

public:
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	UBTT_MoveToPlayer();
};
