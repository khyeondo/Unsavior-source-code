// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileMovementFunction.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UProjectileMovementFunction::UProjectileMovementFunction()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UProjectileMovementFunction::BeginPlay()
{
	
	Super::BeginPlay();

	
	// ...	
}


// Called every frame
void UProjectileMovementFunction::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UProjectileMovementFunction::InitVelocity(FVector _Velocity)
{
	Velocity = _Velocity;
}

void UProjectileMovementFunction::MoveForward(float Speed, float DaltaTime)
{
}

void UProjectileMovementFunction::Seek(FVector TargetPos, float _MaxSpeed)
{
	FinalForce += Seek_Imple(TargetPos, _MaxSpeed);
	//GetOwner()->AddActorWorldOffset(Seek_Imple(TargetPos, MaxSpeed)* DeltaTime);
}

void UProjectileMovementFunction::Homing(FVector TargetPos, float HomingAcceleration, float _MaxSpeed, float DaltaTime)
{
	if (FVector::DotProduct(Velocity.GetSafeNormal(), (TargetPos - GetOwner()->GetActorLocation()).GetUnsafeNormal()) < 0.f)
		return;
		
	//FinalForce += Seek_Imple(TargetPos, MaxSpeed);
	FVector df = (Velocity + (TargetPos - GetOwner()->GetActorLocation()).GetUnsafeNormal() * HomingAcceleration * DaltaTime);
	FVector sf = df.GetSafeNormal() * MaxSpeed;
	FinalForce += sf;
	
	//FVector df = (TargetPos - GetOwner()->GetActorLocation()).GetSafeNormal() * MaxSpeed;
	//FVector sf = df - Velocity;
	//FinalForce += sf * HomingAcceleration;
}

void UProjectileMovementFunction::ApplyForce(float DeltaTime)
{
	Velocity += FinalForce;
	if (Velocity.Size() > MaxSpeed)
	{
		Velocity.Normalize();
		Velocity *= MaxSpeed;
	}

	if (IsOnXYPlane)
		Velocity.Z = 0.f;

	GetOwner()->AddActorWorldOffset(Velocity * DeltaTime);
	FinalForce = FVector::ZeroVector;

	
	if (IsLootAtMoveDir)
	{
		FRotator rotator = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + Velocity);
		GetOwner()->SetActorRotation(rotator);
	}
}

FVector UProjectileMovementFunction::Seek_Imple(FVector TargetPos, float _MaxSpeed)
{
	FVector df = (TargetPos - GetOwner()->GetActorLocation()).GetSafeNormal() * MaxSpeed;
	FVector sf = df - Velocity;
	return sf;
}

