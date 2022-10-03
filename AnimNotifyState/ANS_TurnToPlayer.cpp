// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_TurnToPlayer.h"
#include "Monster.h"

void UANS_TurnToPlayer::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	AMonster* monster = Cast<AMonster>(MeshComp->GetOwner());

	if (IsValid(monster))
	{
		monster->TurnToPlayer(Speed);
	}
}