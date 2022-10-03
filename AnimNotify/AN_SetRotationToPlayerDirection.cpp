// Fill out your copyright notice in the Description page of Project Settings.

#include "AN_SetRotationToPlayerDirection.h"
#include "Monster.h"

void UAN_SetRotationToPlayerDirection::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AMonster* Monster = Cast<AMonster>(MeshComp->GetOwner());
	
	if(IsValid(Monster))
	{
		Monster->SetRotationToPlayerDirection();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AN_SetRotationToPlayerDirection::Notify(): Owner is not Monster"));
	}
}