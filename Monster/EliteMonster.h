// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster.h"
#include "EliteMonster.generated.h"

USTRUCT(BlueprintType)
struct FAttackMontageWithWaitTime
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaitTime = 0.f;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UAnimMontage* AttackMontage;
}; 

USTRUCT(BlueprintType)
struct FMonsterAttack
{
	GENERATED_BODY()

	FMonsterAttack() {
		Timer = CoolTime + 1.f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackRange = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CoolTime = 0.f;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<FAttackMontageWithWaitTime> AttackMontage;		
	UPROPERTY(BlueprintReadOnly)
	float Timer;
};

UCLASS()
class TEAMPROJECT_API AEliteMonster : public AMonster
{
	GENERATED_BODY()

private:
	FMonsterAttack* AttackSchedule = nullptr;
	float AttackComboTimer = 0.0f;
	uint16 AttackComboCounter = 0;

	bool IsAttacking = false;

protected:
	UPROPERTY(EditAnywhere)
	UAnimMontage* TurnRight90;
	UPROPERTY(EditAnywhere)
	UAnimMontage* TurnRight180;
	UPROPERTY(EditAnywhere)
	UAnimMontage* TurnLeft90;
	UPROPERTY(EditAnywhere)
	UAnimMontage* TurnLeft180;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TMap<FString, FMonsterAttack> MonsterAttacks;

	UPROPERTY(BlueprintReadWrite)
	bool IsIdle = true;

public:	
	AEliteMonster();
	
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SideStep(bool IsRight);
	UFUNCTION(BlueprintCallable)
	UAnimMontage* Turn();

	UFUNCTION(BlueprintCallable)
	void PlayAttack(FString Key);
	UFUNCTION(BlueprintPure)
	float GetAttackRangeByKey(FString Key);
	UFUNCTION(BlueprintPure)
	bool IsAttackCoolDown(FString Key);
	UFUNCTION(BlueprintCallable)
	bool IsPlayingAttack();
	
	bool IsPlayerOnRightSide();

	virtual void OnDie_Implementation() override;
	virtual void DieProduction() override;

	virtual bool ApplyDamageWithHitResult_Implementation(float Amount, float Critical, bool IsPlayHitMontage, const FHitResult& SweepResult) override;

	virtual void KnockBack(float Amount, FVector From, FVector Additional);

private:
	void AttackProcess(float DeltaTime);

};
