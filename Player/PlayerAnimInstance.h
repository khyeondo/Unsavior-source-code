// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ControlledCharacterAnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UPlayerAnimInstance : public UControlledCharacterAnimInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly)
	class APlayerCharacter* Player;
	UPROPERTY(BlueprintReadOnly)
	EWeaponType WeaponType;

	UPROPERTY(BlueprintReadOnly)
		bool IsDie = false;

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
public:
	void SetIsDie(bool b);
};
