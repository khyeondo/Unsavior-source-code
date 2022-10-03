// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_TurnToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UANS_TurnToPlayer : public UAnimNotifyState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Speed = 1.f;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
};
