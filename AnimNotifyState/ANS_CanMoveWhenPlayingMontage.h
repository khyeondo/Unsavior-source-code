// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_CanMoveWhenPlayingMontage.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UANS_CanMoveWhenPlayingMontage : public UAnimNotifyState
{
	GENERATED_BODY()
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;	
};
