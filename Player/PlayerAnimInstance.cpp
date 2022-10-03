// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Player = Cast<APlayerCharacter>(Owner);
}
void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (IsValid(Player))
	{
		if (Player->GetEquippingWeapon() != nullptr)
		{
			WeaponType = Player->GetEquippingWeapon()->GetType();
		}
	}
}

void UPlayerAnimInstance::SetIsDie(bool b)
{
	IsDie = b;
}
