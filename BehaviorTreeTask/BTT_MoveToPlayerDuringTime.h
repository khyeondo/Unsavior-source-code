// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MoveToPlayerDuringTime.generated.h"


UCLASS()
class TEAMPROJECT_API UBTT_MoveToPlayerDuringTime : public UBTTaskNode
{
	GENERATED_BODY()

	class AMonster* Monster;
	float Timer;

public:
	UPROPERTY(EditAnywhere)
		float Time;
	UPROPERTY(EditAnywhere)
		bool IsWalk = false;
	UPROPERTY(EditAnywhere)
		float TurnSpeed = 1.f;

protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

public:
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	UBTT_MoveToPlayerDuringTime();


};
