// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAnimInstance.h"
#include "Monster.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMonsterAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();

    Monster = Cast<AMonster>(Owner);
}

void UMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!IsValid(Monster))
    {
        Monster = Cast<AMonster>(Owner);
    }
}

void UMonsterAnimInstance::SetIsDie(bool b)
{
    IsDie = b;
}
