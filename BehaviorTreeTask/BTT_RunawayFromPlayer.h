// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_RunawayFromPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UBTT_RunawayFromPlayer : public UBTTaskNode
{
	GENERATED_BODY()

private:
	UPROPERTY()
		class AMonster* Monster_Owner;

	UPROPERTY(EditAnywhere)
		bool IsWalk = false;
	UPROPERTY(EditAnywhere)
		float TurnSpeed = 1.f;
	UPROPERTY(EditAnywhere)
		float TimeLimit = -1.f;

	float Timer;

protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

public:
	UBTT_RunawayFromPlayer();

	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	
};
