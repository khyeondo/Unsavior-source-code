// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAction.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"

UWeaponAction::UWeaponAction()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponAction::ExcuteTrigger()
{
    if (IsCooldown == false)
    {
        IsCooldown = true;
        Timer = CoolTime;
        IsTriggered = true;
        Trigger();
    }
}

void UWeaponAction::ExcuteTick()
{
    if (IsTriggered == true)
    {
        Tick();

        if (UGameplayStatics::IsGamePaused(this))
        {
            IsTriggered = false;
            Released();
        }
    }    
}

void UWeaponAction::ExcuteReleased()
{
    if (IsTriggered)
    {
        IsTriggered = false;
        Released();
    }
}

void UWeaponAction::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (IsCooldown == true)
    {
        Timer -= DeltaTime;

        if (Timer <= 0.f)
        {
            IsCooldown = false;
            Timer = 0.f;
        }
    }

    ExcuteTick();
}

float UWeaponAction::GetCoolTime()
{
    return CoolTime;
}

float UWeaponAction::GetTimer()
{
    return Timer;
}

bool UWeaponAction::GetIsCooldown()
{
    return IsCooldown;
}