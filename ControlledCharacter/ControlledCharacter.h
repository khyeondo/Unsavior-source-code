// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ControlledCharacter.generated.h"

UENUM(BlueprintType)
enum class EStatus : uint8
{
	MaxHP = 0 UMETA(DisplayName = "MaxHP"),
	HP UMETA(DisplayName = "HP"),
	Speed UMETA(DisplayName = "Speed"),
	Armor UMETA(DisplayName = "Armor"),
	AD UMETA(DisplayName = "AD"),
	Critical UMETA(DisPlayName = "CriticalRate"),
	ExpIncreaseRate UMETA(DisplayName = "ExpIncreaseRate"),

	Burn UMETA(DisplayName = "Burn"),
	Freezing UMETA(DisplayName = "Freezing"),
	Electric UMETA(DisplayName = "Electric"),

	Count  UMETA(DisplayName = "Count")
};

UENUM(BlueprintType)
enum class EDebuff : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Burn UMETA(DisplayName = "Burn"),
	Freezing UMETA(DisplayName = "Freeze"),
	Electric UMETA(DisplayName = "Electric"),
	
	Count
};

UCLASS()
class TEAMPROJECT_API AControlledCharacter : public ACharacter
{
	GENERATED_BODY()

	friend class UControlledCharacterAnimInstance;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* TargetPoint;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Particle)
		TMap<EDebuff, class UNiagaraSystem*> HitEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Particle)
		TMap<EDebuff, class UParticleSystem*> DebuffEffects;
	TMap<EDebuff, class UParticleSystemComponent*> CurrentDebuffEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
		TMap<EDebuff, class USoundBase*> DebuffSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
		class USoundBase* GettingHitSound;

	//class UControlledCharacterAnimInstance* AnimInstance;
	class AController* Controller;

	UPROPERTY(EditAnywhere, Category = Status)
		TMap<EStatus, float> Status;
	UPROPERTY()
		TMap<EStatus, float> StatusAddValue;
	UPROPERTY()
		TMap<EStatus, float> StatusMulValue;
	UPROPERTY(VisibleAnywhere, Category = Status)
		TMap<EStatus, float> StatusResultValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Debuff)
	TArray<float> DebuffTime;
	UPROPERTY(BlueprintReadOnly, Category = Debuff)
	TArray<float> InitialDebuffTime;
	
	float InitialMoveSpeed;

	UPROPERTY(BlueprintReadOnly)
	bool IsDie;

	UPROPERTY(BlueprintReadOnly)
	bool CanMove;
	bool CanAttack;

	bool IsWalk;

	float GroundHeight;

	class UAnimInstance* AnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	class UAnimMontage* HitMontage;

private:
	float RunAnimationPlayRate;

public:
	// Sets default values for this character's properties
	AControlledCharacter();

protected:
	virtual void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializedValue();

	void DrawDebugStatus();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void SubHP(float f);
	virtual void SubHP_Implementation(float f);

	UFUNCTION(BlueprintNativeEvent)
		void OnDie();
	virtual void OnDie_Implementation();

	UFUNCTION(BlueprintCallable)
		void SetDebuffTime(EDebuff debuffType, float time);
	UFUNCTION(BlueprintCallable)
		void AddDebuffTime(EDebuff debuffType, float time);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void OnBeginDebuff(EDebuff debuffType);
	virtual void OnBeginDebuff_Implementation(EDebuff debuffType);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void OnTickDebuff(EDebuff debuffType, float deltaTime);
	virtual void OnTickDebuff_Implementation(EDebuff debuffType, float deltaTime);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void OnEndDebuff(EDebuff debuffType);
	virtual void OnEndDebuff_Implementation(EDebuff debuffType);


	UFUNCTION(BlueprintCallable)
		void SetStatus(EStatus type, float value);
	UFUNCTION(BlueprintPure)
		float GetStatus(EStatus type);
	UFUNCTION(BlueprintCallable)
		void AddStatus(EStatus type, float value);
	UFUNCTION(BlueprintCallable)
		void AddStatusAddValue(EStatus type, float value);
	UFUNCTION(BlueprintCallable)
		void AddStatusMulValue(EStatus type, float value);
	UFUNCTION(BlueprintNativeEvent)
		void OnChangeStatus(EStatus type);
	void OnChangeStatus_Implementation(EStatus type);
	UFUNCTION(BlueprintPure)
		float GetStatusMulValue(EStatus type);
	UFUNCTION(BlueprintPure)
	virtual float GetStatusResultValue(EStatus type);
	UFUNCTION(BlueprintPure)
	bool GetIsWalk();

	float GetDebuffStatusResultValue(EDebuff Debuff);

	UFUNCTION(BlueprintPure)
		bool GetCanMove();

	UFUNCTION(BlueprintCallable)
		virtual void DisableAttackCollision();
	UFUNCTION(BlueprintCallable)
		virtual void EnableAttackCollision();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ApplyDamage(float Amount, float Critical, bool IsPlayHitMontage);
	virtual bool ApplyDamage_Implementation(float Amount, float Critical, bool IsPlayHitMontage);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ApplyDamageWithHitResult(float Amount, float Critical, bool IsPlayHitMontage, const FHitResult& SweepResult);
	virtual bool ApplyDamageWithHitResult_Implementation(float Amount, float Critical, bool IsPlayHitMontage, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnHit();
	virtual void OnHit_Implementation();

	UFUNCTION(BlueprintCallable)
	virtual void KnockBack(float Amount,FVector From, FVector Additional);

	UFUNCTION(BlueprintCallable)
		void ApplyDebuff(EDebuff Debuff, float StatusValue);		

	UFUNCTION(BlueprintCallable)
		void SetIsWalk(bool b);

	UFUNCTION()
		bool GetIsDie();

private:
	void BeginBurning();
	void BeginFreezing();
	void BeginElectrocuting();
	void BeginBleeding();
	void BeginFractured();

	void Burning(float deltaTime);
	void Freezing(float deltaTime);
	void Electrocuting(float deltaTime);
	void Bleeding(float deltaTime);
	void Fractured(float deltaTime);

	void EndBurning();
	void EndFreezing();
	void EndElectrocuting();
	void EndBleeding();
	void EndFractured();
};
