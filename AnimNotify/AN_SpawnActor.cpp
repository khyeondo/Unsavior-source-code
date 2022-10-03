// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_SpawnActor.h"
#include "HitBoxComponent.h"
#include "Kismet/GameplayStatics.h"

void UAN_SpawnActor::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!IsValid(MeshComp->GetOwner()))
	{
		return;
	}

	AActor* Actor;
	if (IsAttached)
	{
		Actor = MeshComp->GetOwner()->GetWorld()->SpawnActor<AActor>(*SpawnType, MeshComp->GetSocketTransform(SocketName));
	}
	else
	{
		Actor = MeshComp->GetOwner()->GetWorld()->SpawnActor<AActor>(*SpawnType, MeshComp->GetOwner()->GetTransform());
	}
	
	//AActor* Actor = MeshComp->GetOwner()->GetWorld()->SpawnActorDeferred<AActor>(*SpawnType, MeshComp->GetOwner()->GetTransform(),nullptr,nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	//TSubclassOf<UHitBoxComponent>();
	if (IsValid(Actor))
	{
		//UActorComponent* HitBoxComponent = Actor->GetComponentByClass(TSubclassOf<UHitBoxComponent>());

		UActorComponent* HitBoxComponent = Actor->GetComponentByClass(UHitBoxComponent::StaticClass());
		if (IsValid(HitBoxComponent))
		{						
			UHitBoxComponent* HitBox = Cast<UHitBoxComponent>(HitBoxComponent);

			if (HitBoxTarget == TEXT("Monster"))
			{
				HitBox->SetTargetToMonster();
			}
			else if (HitBoxTarget == TEXT("Player"))
			{
				HitBox->SetTargetToPlayer();
			}

			if (IsValid(MeshComp->GetOwner<AControlledCharacter>())) {
				HitBox->SetDamageInfo(MeshComp->GetOwner<AControlledCharacter>());
			}
		}
	}

	//UGameplayStatics::FinishSpawningActor(
	//	Actor,
	//	MeshComp->GetOwner()->GetTransform()
	//);

}
