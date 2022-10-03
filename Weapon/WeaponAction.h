// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponAction.generated.h"

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class TEAMPROJECT_API UWeaponAction : public UActorComponent
{
    GENERATED_BODY()

private:
    float Timer = 0.f;
    bool IsTriggered = false;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class UAnimMontage* Anim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float CoolTime;
    UPROPERTY(BlueprintReadOnly)
        bool IsCooldown = false;

public:
    UWeaponAction();

    void ExcuteTrigger();
    void ExcuteTick();
    void ExcuteReleased();

    UFUNCTION(BlueprintNativeEvent)
    void Trigger();
    virtual void Trigger_Implementation() {}
    UFUNCTION(BlueprintNativeEvent)
    void Tick();
    virtual void Tick_Implementation() {}
    UFUNCTION(BlueprintNativeEvent)
    void Released();
    virtual void Released_Implementation() {}

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    float GetCoolTime();
    float GetTimer();
    bool GetIsCooldown();
 
};
