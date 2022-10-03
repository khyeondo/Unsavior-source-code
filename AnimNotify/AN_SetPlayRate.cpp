// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_SetPlayRate.h"

void UAN_SetPlayRate::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Animation->RateScale = PlayRate;
}
