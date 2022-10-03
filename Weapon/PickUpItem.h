// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpItem.generated.h"

UCLASS()
class TEAMPROJECT_API APickUpItem : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere)
		class UHitBoxComponent* HitBox;

protected:
	UPROPERTY(BlueprintReadOnly)
		class APlayerCharacter* Player;

public:	
	// Sets default values for this actor's properties
	APickUpItem();

	UFUNCTION(BlueprintNativeEvent)
	void OnPickUp();
	void OnPickUp_Implementation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
		void OnHitCollisionBeginOverlap(class AControlledCharacter* OtherActor, const FHitResult& SweepResult);
};
