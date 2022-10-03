// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ControlledCharacterAnimInstance.h"
#include "MonsterAnimInstance.generated.h"

UCLASS()
class TEAMPROJECT_API UMonsterAnimInstance : public UControlledCharacterAnimInstance
{
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadOnly)
        class AMonster* Monster;

    UPROPERTY(BlueprintReadOnly)
    bool IsDie = false;

protected:
    virtual void NativeBeginPlay() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds);

public:
    void SetIsDie(bool b);
};