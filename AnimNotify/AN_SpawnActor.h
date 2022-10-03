// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SpawnActor.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJECT_API UAN_SpawnActor : public UAnimNotify
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> SpawnType;

	UPROPERTY(EditAnywhere)
	bool IsAttached = false;
	UPROPERTY(EditAnywhere)
	FName SocketName;
	
	UPROPERTY(EditAnywhere)
	FString HitBoxTarget;



	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
