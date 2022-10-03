// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_CanMoveWhenPlayingMontage.h"
#include "PlayerCharacter.h"

void UANS_CanMoveWhenPlayingMontage::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	APlayerCharacter* Player = MeshComp->GetOwner<APlayerCharacter>();

	if (IsValid(Player))
	{
		Player->SetIsWalk(false);
		Player->Movement(FrameDeltaTime);
	}
}
