// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_EliteTurnToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UBTT_EliteTurnToPlayer : public UBTTaskNode
{
	GENERATED_BODY()
private:	
	class UAnimMontage* Montage = nullptr;
	class UAnimInstance* AnimInstance = nullptr;
	float Timer;

public:
	UBTT_EliteTurnToPlayer();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
	
};
