

#include "ANS_PlayerTurnDuringAttack.h"
#include "PlayerCharacter.h"

void UANS_PlayerTurnDuringAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	APlayerCharacter* player = Cast<APlayerCharacter>(MeshComp->GetOwner());

	if (IsValid(player))
	{
		player->FindMoveDirection();
		player->SetPlayerDirection(FrameDeltaTime, 10.f);		
	}

}
