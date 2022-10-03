// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SetPlayRate.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UAN_SetPlayRate : public UAnimNotify
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float PlayRate = 1.f; 

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
