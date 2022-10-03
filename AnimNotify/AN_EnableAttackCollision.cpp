// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_EnableAttackCollision.h"
#include "Monster.h"

void UAN_EnableAttackCollision::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AMonster* Monster = Cast<AMonster>(MeshComp->GetOwner());

	if (IsValid(Monster))
	{
		Monster->EnableAttackCollision();
	}
}