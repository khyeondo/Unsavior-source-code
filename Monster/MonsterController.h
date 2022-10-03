// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterController.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API AMonsterController : public AAIController
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
		class UBehaviorTree* BT_Monster;
	UPROPERTY(EditAnywhere)
		class UBlackboardData* BB_Monster;
	UPROPERTY()
		class AMonster* TaskOwnedMonster;

	bool IsPathFollowing = false;
public:
	static const FName Key_TaskOwner;

public:
	AMonsterController();

	bool GetIsPathFollowing();

	void SetActive(bool b);

	virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr) override;

protected:
	void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	void RunAI();

};
