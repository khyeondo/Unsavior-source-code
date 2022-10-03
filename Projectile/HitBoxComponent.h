// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "ControlledCharacter.h"
#include "HitBoxComponent.generated.h"


#define PlayerCollisionChannel ECollisionChannel::ECC_GameTraceChannel1
#define MonsterCollisionChannel ECollisionChannel::ECC_GameTraceChannel2

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeleOnSetDamageInfo, AControlledCharacter*, Spawner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDeleOnHitColliderBeginOverlap, class AControlledCharacter*, OtherActor, const FHitResult&, SweepResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDeleOnHitColliderOverlapping, AControlledCharacter*, OtherActor, float, DeltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeleOnHitColliderEndOverlap, AControlledCharacter*, OtherActor);



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAMPROJECT_API UHitBoxComponent : public UCapsuleComponent
{
	GENERATED_BODY()

private:
	TSet<AActor*> OverlappingActors;
	TSet<AActor*> OverlappedActors;

	UPROPERTY(EditAnywhere)
	bool IsCheckBeginOverlapOnce;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FString TargetType;	
	UPROPERTY(BlueprintReadOnly)
	class AActor* TargetActor = nullptr;
	UPROPERTY(BlueprintReadOnly)
		float Damage;
	UPROPERTY(BlueprintReadOnly)
		float StatusCriticalValue;
	UPROPERTY(BlueprintReadOnly)
		float StatusDebuffValue;
	UPROPERTY(BlueprintReadOnly)
		EDebuff Debuff;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool UseHitResult = false;

	UPROPERTY(BlueprintReadOnly)
		bool IsSetDamage = false;

public:
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event", Meta = (DisplayName = "OnHitColliderOverlapBegin"))
		FDeleOnHitColliderBeginOverlap Func_HitColliderBeginOverlap;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event", Meta = (DisplayName = "OnHitColliderOverlapping"))
		FDeleOnHitColliderOverlapping Func_HitColliderOverlapping;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event", Meta = (DisplayName = "OnHitColliderEndOverlap"))
		FDeleOnHitColliderEndOverlap Func_HitColliderEndOverlap;	

	UPROPERTY(BlueprintAssignable,VisibleAnywhere, BlueprintCallable, Category = "Event", Meta = (DisplayName = "OnSpawnedByActor"))
	FDeleOnSetDamageInfo Func_OnSpawnedByActor;

public:
	// Sets default values for this component's properties
	UHitBoxComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostLoad() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION()
		void OnHitColliderOverlapBegin(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);

	UFUNCTION()
		void OnHitColliderEndOverlap(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
		);


	UFUNCTION(BlueprintCallable)
		void SetUseHitResult(bool b);
	UFUNCTION(BlueprintCallable)
		void SetTargetToPlayer();
	UFUNCTION(BlueprintCallable)
	void SetTargetToMonster();

	UFUNCTION(BlueprintCallable)
	void SetDamageInfoCustom(float _Damage, FString _TargetType, AActor* _TargetActor = nullptr, float _StatusCriticalValue = 0, float _StatusDebuffValue =0, EDebuff _Debuff = EDebuff::None);
	UFUNCTION(BlueprintCallable)
	void SetDamageInfo(AControlledCharacter* Spawner);
	UFUNCTION(BlueprintCallable)
	void SetDamageInfoByHitBox(UHitBoxComponent* HitBox);

	UFUNCTION(BlueprintPure)
		FString GetTargetType();
	UFUNCTION(BlueprintPure)
		AActor* GetTargetActor();
	UFUNCTION(BlueprintPure)
		float GetDamage();
	UFUNCTION(BlueprintPure)
		float GetCriticalProbaility();
	UFUNCTION(BlueprintPure)
		float GetDebuffProbaility();
	UFUNCTION(BlueprintPure)
		EDebuff GetDebuff();


	UFUNCTION(BlueprintCallable)
		void EnableCollision();
	UFUNCTION(BlueprintCallable)
		void DisableCollision();

	UFUNCTION(BlueprintCallable)
	void EmptyOverlappedActors();
	UFUNCTION(BlueprintCallable)
	void SetIsCheckBeginOverlapOnce(bool b);
};
