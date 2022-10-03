// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_EliteTurnToPlayer.h"
#include "EliteMonster.h"
#include "AIController.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

UBTT_EliteTurnToPlayer::UBTT_EliteTurnToPlayer()
{
	NodeName = TEXT("ElitTurnToPlayer");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_EliteTurnToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Montage = nullptr;
	Montage = OwnerComp.GetAIOwner()->GetPawn<AEliteMonster>()->Turn();
	AnimInstance = OwnerComp.GetAIOwner()->GetCharacter()->GetMesh()->GetAnimInstance();	

	if (Montage != nullptr)
	{
		Timer = Montage->SequenceLength / Montage->RateScale;		
		return EBTNodeResult::Type::InProgress;
	}
	else
	{
		return EBTNodeResult::Type::Succeeded;
	}	
}

void UBTT_EliteTurnToPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Timer -= DeltaSeconds;
	
	if (Timer <= 0.f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	//if (AnimInstance->GetCurrentActiveMontage() != Montage)
	//{
	//	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	//}
}
