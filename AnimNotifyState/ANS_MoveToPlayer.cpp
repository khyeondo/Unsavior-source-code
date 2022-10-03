// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_MoveToPlayer.h"
#include "Monster.h"
#include "PlayerCharacter.h"

void UANS_MoveToPlayer::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UANS_MoveToPlayer::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	AMonster* monster = Cast<AMonster>(MeshComp->GetOwner());	
	if (IsValid(monster))
	{
		monster->AddActorWorldOffset(
			(monster->GetPlayer()->GetActorLocation() - monster->GetActorLocation()).GetSafeNormal() * Speed * FrameDeltaTime * FVector(1, 1, 0)
		);
		//monster->AddActorLocalOffset(
		//	 (monster->GetPlayer()->GetActorLocation() - monster->GetActorLocation()).GetSafeNormal() * Speed * FrameDeltaTime * FVector(1,1,0)
		//);

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("ANS_MoveToPlayer::NotifyTick() Player is not valid"));
	}

	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("ANS_MoveToPlayer is not working"));
}

void UANS_MoveToPlayer::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

}
