// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_TurnToPlayerByCurve.h"
#include "Kismet/KismetMathLibrary.h"
#include "EliteMonster.h"
#include "PlayerCharacter.h"

void UANS_TurnToPlayerByCurve::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	TargetAngle = FRotator::ZeroRotator;
}

void UANS_TurnToPlayerByCurve::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	float CurveValue;
	MeshComp->GetAnimInstance()->GetCurveValue(TEXT("TurnCurve"), CurveValue);

	if (CurveValue != 0.0f)
	{
		AEliteMonster* Monster = MeshComp->GetOwner<AEliteMonster>();
		if (IsValid(Monster))
		{
			if (TargetAngle.IsZero())
			{
				APlayerCharacter* Player = Monster->GetPlayer();

				FVector location = Monster->GetActorLocation() * FVector(1.f, 1.f, 0.f);
				FVector playerLocation = Player->GetActorLocation() * FVector(1.f, 1.f, 0.f);
				FRotator lookAtRotator = UKismetMathLibrary::FindLookAtRotation(location, playerLocation);
				OriginAngle = Monster->GetActorRotation();
				TargetAngle = lookAtRotator;
			}
			else
			{
				Monster->SetActorRotation(UKismetMathLibrary::RLerp(OriginAngle, TargetAngle, CurveValue, true));
			}
		}
	}
}
