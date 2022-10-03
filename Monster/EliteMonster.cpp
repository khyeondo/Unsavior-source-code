// Fill out your copyright notice in the Description page of Project Settings.


#include "EliteMonster.h"
#include "MonsterController.h"
#include "PlayerCharacter.h"
#include "MonsterAnimInstance.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


AEliteMonster::AEliteMonster()
{
	PrimaryActorTick.bCanEverTick = true;	
}

void AEliteMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (AttackSchedule != nullptr)
	{
		AttackProcess(DeltaTime);
	}

	for (auto& attack : MonsterAttacks)
	{
		attack.Value.Timer += DeltaTime;
	}
}

void AEliteMonster::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	IsCanPushed = false;
	for (auto& attack : MonsterAttacks)
	{
		attack.Value.Timer += attack.Value.CoolTime + 1.f;
	}
}

void AEliteMonster::SideStep(bool IsRight)
{
	if (IsRight)
	{
		AIController->MoveToLocation(GetActorLocation() + GetActorRightVector() * 100.f);
	}
	else
	{
		AIController->MoveToLocation(GetActorLocation() - GetActorRightVector() * 100.f);
	}
}

UAnimMontage* AEliteMonster::Turn()
{
	float Angle =
		 GetActorForwardVector().CosineAngle2D(Player->GetActorLocation() - GetActorLocation());

	Angle = FMath::Acos(Angle);
	Angle = FMath::RadiansToDegrees(Angle);

	if (Angle < 60.f)
		return nullptr;

	if (IsPlayerOnRightSide())
	{
		if (Angle > 150.f) {
			if (IsValid(TurnRight180))
			{
				AnimInstance->Montage_Play(TurnRight180);
				return TurnRight180;
			}
			else if (IsValid(TurnRight90)) {
				AnimInstance->Montage_Play(TurnRight90);
				return TurnRight90;
			}
		}
		else {
			if (IsValid(TurnRight90)){
				AnimInstance->Montage_Play(TurnRight90);
				return TurnRight90;
			}
				
			else if (IsValid(TurnRight180)){
				AnimInstance->Montage_Play(TurnRight180);
				return TurnRight180;
			}
		}

	}
	else
	{
		if (Angle > 150.f) {
			if (IsValid(TurnLeft180)) {
				AnimInstance->Montage_Play(TurnLeft180);
				return TurnLeft180;
			}
			else if (IsValid(TurnLeft90)) {
				AnimInstance->Montage_Play(TurnLeft90);
				return TurnLeft90;
			}
		}
		else{
			if (IsValid(TurnLeft90)) {
				AnimInstance->Montage_Play(TurnLeft90);
				return TurnLeft90;
			}
			else if (IsValid(TurnLeft180)) {
				AnimInstance->Montage_Play(TurnLeft180);
				return TurnLeft180;
			}
		}			
	}


	return nullptr;
}

void AEliteMonster::PlayAttack(FString Key)
{
	if (IsAttacking == false)
	{
		
		if (!MonsterAttacks.Contains(Key))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,FString::Printf(TEXT("AEliteMonster::PlayAttack():%s is invalid key."), *Key));
			return;
		}

		AttackSchedule = &(MonsterAttacks[Key]);
		IsAttacking = true;

		if (IsValid(AttackSchedule->AttackMontage[AttackComboCounter].AttackMontage))
		{
			MonsterAttacks[Key].Timer = 0.f;
			AnimInstance->Montage_Play(AttackSchedule->AttackMontage[AttackComboCounter].AttackMontage);
			AttackComboTimer = AttackSchedule->AttackMontage[AttackComboCounter].AttackMontage->GetPlayLength()
				/ AttackSchedule->AttackMontage[AttackComboCounter].AttackMontage->RateScale;
		}
		else
		{
			AttackComboTimer = 1.f;
		}
	}
}

float AEliteMonster::GetAttackRangeByKey(FString Key)
{
	if (MonsterAttacks.Contains(Key))
	{
		return MonsterAttacks[Key].AttackRange;
	}

	return -1.f;
}

bool AEliteMonster::IsAttackCoolDown(FString Key)
{
	if(MonsterAttacks.Contains(Key))
		return MonsterAttacks[Key].CoolTime < MonsterAttacks[Key].Timer;
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AEliteMonster::IsAttackCoolDown(): Incorrect key value "));;
		return false;
	}
}

bool AEliteMonster::IsPlayingAttack()
{
	return IsAttacking;
}

bool AEliteMonster::IsPlayerOnRightSide()
{
	FVector DirToPlayer = Player->GetActorLocation() - GetActorLocation();

	FVector CrossVector = FVector::CrossProduct(GetActorForwardVector(), DirToPlayer);
	
	return FVector::DotProduct(FVector::UpVector, CrossVector) > 0.f;
}

void AEliteMonster::OnDie_Implementation()
{
	Super::OnDie_Implementation();
}

void AEliteMonster::DieProduction()
{
	Cast<UMonsterAnimInstance>(AnimInstance)->SetIsDie(true);
	AIController->UnPossess();
	AnimInstance->Montage_Stop(0.5f);

	SetActorEnableCollision(false);
}

bool AEliteMonster::ApplyDamageWithHitResult_Implementation(float Amount, float Critical, bool IsPlayHitMontage, const FHitResult& SweepResult)
{
	return AControlledCharacter::ApplyDamageWithHitResult_Implementation(Amount, Critical, IsPlayHitMontage, SweepResult);
}

void AEliteMonster::KnockBack(float Amount, FVector From, FVector Additional)
{
}

void AEliteMonster::AttackProcess(float DeltaTime)
{		
	AttackComboTimer -= DeltaTime;

	if (AttackComboTimer <= -1.f*AttackSchedule->AttackMontage[AttackComboCounter].WaitTime)
	{
		AttackComboCounter++;
		if (AttackComboCounter >= AttackSchedule->AttackMontage.Num())
		{
			AttackSchedule = nullptr;
			IsAttacking = false;
			AttackComboTimer = 0.f;
			AttackComboCounter = 0;
			return;
		}		

		if (IsValid(AttackSchedule->AttackMontage[AttackComboCounter].AttackMontage))
		{
			AnimInstance->Montage_Play(AttackSchedule->AttackMontage[AttackComboCounter].AttackMontage);
			AttackComboTimer = AttackSchedule->AttackMontage[AttackComboCounter].AttackMontage->GetPlayLength()
				/ AttackSchedule->AttackMontage[AttackComboCounter].AttackMontage->RateScale;
		}
		else
		{
			AttackComboTimer = 1.f;
		}
	}			
}
