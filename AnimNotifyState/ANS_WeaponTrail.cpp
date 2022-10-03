// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_WeaponTrail.h"
#include "Weapon.h"
#include "PlayerCharacter.h"

void UANS_WeaponTrail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	APlayerCharacter* player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if (IsValid(player))
	{
		Weapon = player->GetEquippingWeapon();
		Weapon->BeginTrail();
	}
}

void UANS_WeaponTrail::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (IsValid(Weapon))
	{
		APlayerCharacter* player = Cast<APlayerCharacter>(MeshComp->GetOwner());
		if (IsValid(player))
		{
			if (Weapon != player->GetEquippingWeapon())
			{
				Weapon->EndTrail();
				Weapon = player->GetEquippingWeapon();
				Weapon->BeginTrail();
			}
		}
	}
}

void UANS_WeaponTrail::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	APlayerCharacter* player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if (IsValid(player))
	{
		Weapon = player->GetEquippingWeapon();
		Weapon->EndTrail();
		Weapon = nullptr;
	}
}
