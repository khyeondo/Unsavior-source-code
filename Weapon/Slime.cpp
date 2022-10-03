// Fill out your copyright notice in the Description page of Project Settings.


#include "Slime.h"
#include "Weapon.h"
#include "PlayerCharacter.h"

// Sets default values
ASlime::ASlime()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Player = nullptr;

	WeaponSlotNum = 3;
	WeaponSlot.Init(nullptr, WeaponSlotNum);
	CurWeaponIdx = 0;
	IsEmpty = true;

}

// Called when the game starts or when spawned
void ASlime::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASlime::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASlime::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ASlime::ChangeWeapon(uint8 idx)
{
	if(!IsValid(Player))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("ASlime::ChangeWeapon(): Player is not valid"));
		return false;
	}

	if (0 > idx || idx > 3)
		return false;

	if (WeaponSlot[idx] == nullptr)
		return false;

	if (idx == CurWeaponIdx)
		return false;

	WeaponSlot[CurWeaponIdx]->OnDisableWeapon();
	CurWeaponIdx = idx;
	WeaponSlot[CurWeaponIdx]->OnEnableWeapon();

	Player->PlayWeaponChangeSound();
	Player->SwitchWeaponAnimList();

	return true;
}

bool ASlime::AddWeapon(TSubclassOf<class AWeapon> weaponType)
{
	//최초 무기 추가 시
	if (WeaponSlot[0] == nullptr)
	{
		AddWeapon_Implementation(0, weaponType, true);

		return true;
	}

	if (IsEmpty) //슬롯에 자리가 남아있으면
	{
		for (size_t i = 0; i < WeaponSlotNum; i++)
		{
			if (WeaponSlot[i] == nullptr)
			{
				AddWeapon_Implementation(i, weaponType, false);

				if (WeaponSlot.Last() != nullptr)		//마지막으로 들어간 무기 추가 처리
				{
					IsEmpty = false;
				}

				return true;
			}
		}
	}
	else //슬롯에 자리가 없으면
	{
		WeaponSlot[CurWeaponIdx]->OnDisableWeapon();
		AddWeapon_Implementation(CurWeaponIdx, weaponType, false);
		WeaponSlot[CurWeaponIdx]->OnEnableWeapon();

		Player->PlayWeaponChangeSound();
		Player->SwitchWeaponAnimList();

		return true;
	}

	return false;
}

void ASlime::AddAltarWeapon(TSubclassOf<class AWeapon> weaponType)
{
	IsEmpty = true;

	WeaponSlot[CurWeaponIdx]->OnDisableWeapon();
	CurWeaponIdx = 0;

	for (size_t i = 0; i < WeaponSlotNum; i++)
	{
		WeaponSlot[i] = nullptr;
	}

	AddWeapon_Implementation(0, weaponType, false);
	WeaponSlot[CurWeaponIdx]->OnEnableWeapon();

	Player->PlayWeaponChangeSound();
	Player->SwitchWeaponAnimList();
}

void ASlime::AddWeapon_Implementation(int index, TSubclassOf<class AWeapon> weaponType, bool b)
{
	FActorSpawnParameters SpanwParameters;
	SpanwParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	WeaponSlot[index] = GetWorld()->SpawnActor<AWeapon>(
		weaponType,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpanwParameters
		);

	if (!IsValid(WeaponSlot[index]))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Slime::AddWeapon_Implementation() : WeaponSlot[index] is not valid"));
		return;
	}


	WeaponSlot[index]->AttachToActor(
		this,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale
	);

	WeaponSlot[index]->SetSlime(this);
	WeaponSlot[index]->SetPlayer(Player);
	WeaponSlot[index]->SetEnableImmediately(b);
}

AWeapon* ASlime::GetEquippingWeapon()
{
	return WeaponSlot[CurWeaponIdx];
}

void ASlime::AttachToPlayer(APlayerCharacter* player)
{
	Player = player;

	 AttachToComponent(
		Player->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		FName("weapon")
	);
}

TArray<class AWeapon*> ASlime::GetWeaponSlot()
{
	return WeaponSlot;
}

bool ASlime::GetIsEmpty()
{
	return IsEmpty;
}