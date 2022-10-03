// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_TurnToPlayerByCurve.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UANS_TurnToPlayerByCurve : public UAnimNotifyState
{
	GENERATED_BODY()

	FRotator TargetAngle;
	FRotator OriginAngle;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration);
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
};
