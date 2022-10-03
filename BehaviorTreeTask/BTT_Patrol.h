// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterController.h"

#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Patrol.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UBTT_Patrol : public UBTTaskNode
{
	GENERATED_BODY()

private:
	class AMonster* Monster;
	class AMonsterController* MonsterController;
	bool IsMoveCompleted;

	UPROPERTY(EditAnywhere)
	bool IsWalk = false;

	UPROPERTY(EditAnywhere)
	float TurnSpeed = 1.f;

	UPROPERTY(EditAnywhere)
		float MinDist = 300.f;
	UPROPERTY(EditAnywhere)
		float MaxDist = 500.f;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)override;

public:
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	UBTT_Patrol();

};
