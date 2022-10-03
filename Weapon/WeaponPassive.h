// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponPassive.generated.h"


UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class TEAMPROJECT_API UWeaponPassive : public UActorComponent
{
	GENERATED_BODY()

protected:

	int a = 0;

public:
	UFUNCTION(BlueprintNativeEvent)
	void OnEquipped();
	virtual void OnEquipped_Implementation() {}
	UFUNCTION(BlueprintNativeEvent)
	void Trigger();
	virtual void Trigger_Implementation() {}
	UFUNCTION(BlueprintNativeEvent)
	void Tick();
	virtual void Tick_Implementation() {}
	UFUNCTION(BlueprintNativeEvent)
	void OnUnequipped();
	virtual void OnUnequipped_Implementation() {}

};