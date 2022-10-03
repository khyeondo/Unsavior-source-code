// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ElitMonsterSideStep.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UBTT_ElitMonsterSideStep : public UBTTaskNode
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	bool IsRight;
	UPROPERTY(EditAnywhere)
	bool RandomDirection;
	UPROPERTY(EditAnywhere)
	bool IsWalk;
	UPROPERTY(EditAnywhere)
	float TurnSpeed = 3.f;


protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

public:
	UBTT_ElitMonsterSideStep();
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
};
