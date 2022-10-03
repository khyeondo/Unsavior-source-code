// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectileMovementFunction.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPROJECT_API UProjectileMovementFunction : public UActorComponent
{
	GENERATED_BODY()

private:
	FVector FinalForce;
	FVector Velocity;

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsOnXYPlane = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsLootAtMoveDir = false;

public:	
	// Sets default values for this component's properties
	UProjectileMovementFunction();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void InitVelocity(FVector _Velocity);

	UFUNCTION(BlueprintCallable)
	void MoveForward(float Speed,float DaltaTime);
	UFUNCTION(BlueprintCallable)
	void Seek(FVector TargetPos, float _MaxSpeed);
	UFUNCTION(BlueprintCallable)
	void Homing(FVector TargetPos,float HomingAcceleration, float _MaxSpeed, float DaltaTime);
	UFUNCTION(BlueprintCallable)
	void ApplyForce(float DeltaTime);

	FVector Seek_Imple(FVector TargetPos, float _MaxSpeed);
};
