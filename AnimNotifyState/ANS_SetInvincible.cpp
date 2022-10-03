// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_SetInvincible.h"
#include "PlayerCharacter.h"


void UANS_SetInvincible::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	APlayerCharacter* Character =  MeshComp->GetOwner<APlayerCharacter>();

	if (IsValid(Character))
	{
		Character->GetHitBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		OriginChannel = Character->GetCapsuleComponent()->GetCollisionObjectType();
		Character->GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	}
}

void UANS_SetInvincible::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	APlayerCharacter* Character = MeshComp->GetOwner<APlayerCharacter>();

	if (IsValid(Character))
	{

		Character->GetHitBox()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		Character->GetCapsuleComponent()->SetCollisionObjectType(OriginChannel);
	}
}
