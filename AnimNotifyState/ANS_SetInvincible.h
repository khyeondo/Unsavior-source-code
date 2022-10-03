// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Components/CapsuleComponent.h"
#include "ANS_SetInvincible.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UANS_SetInvincible : public UAnimNotifyState
{
	GENERATED_BODY()

	ECollisionChannel OriginChannel;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration);
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);	
};
