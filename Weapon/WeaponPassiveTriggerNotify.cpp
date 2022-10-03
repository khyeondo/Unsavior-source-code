// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponPassiveTriggerNotify.h"
#include "PlayerCharacter.h"
#include "Weapon.h"
#include "WeaponPassive.h"

void UWeaponPassiveTriggerNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	APlayerCharacter* player = Cast<APlayerCharacter>(MeshComp->GetOwner());

	if (IsValid(player))
	{
		player->GetEquippingWeapon()->ExcuteWeaponPassiveTrigger();
	}
	
	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Purple, TEXT("passive trigger notify"));
}