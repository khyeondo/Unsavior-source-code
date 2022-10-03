// Fill out your copyright notice in the Description page of Project Settings.// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ControlledCharacter.h"
//#include "Navigation/PathFollowingComponent.h"
#include "Monster.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeleOnDie);

UENUM(Meta = (Bitflags))
enum class EMonsterDebugOptions
{
	None = 0b000001 << 1, //== 0b000010
	Status = 0b000001 << 2, //== 0b000100
	AttackRange = 0b000001 << 4, //== 0b010000

	Count
};

ENUM_CLASS_FLAGS(EMonsterDebugOptions)

UCLASS()
class TEAMPROJECT_API AMonster : public AControlledCharacter
{
	GENERATED_BODY()


protected:
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event", Meta = (DisplayName = "OnDieEvent"))
		FDeleOnDie Func_OnDie;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UWidgetComponent* HealthWidgetComp;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//	class UHitBoxComponent* HitBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<class UHitBoxComponent*> MeleeAttackHitBoxList;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FName Name;
	UPROPERTY(EditAnywhere)
		EDebuff Element;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AttackRange;

	UPROPERTY(BlueprintReadOnly)
		class APlayerCharacter* Player;
	UPROPERTY(BlueprintReadOnly)
		class AMonsterController* AIController;
	//class UAnimInstance* AnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* AttackMontage;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	//class UAnimMontage* HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Particle)
		class UParticleSystem* DeathEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SoulDrop)
		TArray<TSubclassOf<class AActor>> SoulOfThisMonster;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SoulDrop)
		float SoulDropChance;

	UPROPERTY(EditAnywhere, Meta = (Bitmask, BitmaskEnum = "EMonsterDebugOptions"))
		int32 DebugOptions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float ExpForPlayer;

	UPROPERTY(EditAnywhere)
		bool Active = true;

	UPROPERTY(EditAnywhere)
		float DestroyTimer = 5.f;

	UPROPERTY(BlueprintReadWrite)
		bool IsCanPushed = true;

public:
	// Sets default values for this character's properties
	AMonster();

protected:
	virtual void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		class AActor* DamageCauser
	) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		void SetActive(bool b);
	UFUNCTION(BlueprintPure)
		bool GetActive();

	UFUNCTION(BlueprintCallable)
		void MoveToPlayer();
	UFUNCTION(BlueprintCallable)
		void MoveToLocation(FVector location);

	UFUNCTION(BlueprintNativeEvent)
		void OnSpawned();
	virtual void OnSpawned_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void Attack();
	virtual void Attack_Implementation();

	UFUNCTION(BlueprintCallable)
		void TurnToMoveDirection(float speed = 1.f);
	UFUNCTION(BlueprintCallable)
		void TurnToPlayer(float Speed);
	UFUNCTION(BlueprintCallable)
		void SetRotationToPlayerDirection();

	UFUNCTION(BlueprintCallable)
		float DotProductToPlayer();

	UFUNCTION(Blueprintpure)
		float DistToPlayer();

	UFUNCTION(Blueprintpure)
		float GetAttackRange();
	APlayerCharacter* GetPlayer();
	UFUNCTION(Blueprintpure)
		EDebuff GetElement();

	virtual void DisableAttackCollision() override;
	virtual void EnableAttackCollision() override;

	UFUNCTION(BlueprintCallable) //###########################
		static AMonster* Spawn(
			const AActor* WorldContextActor,
			FString Id,
			FName _Name,
			float MaxHp,
			float Hp,
			float Speed,
			float AD,
			float Armor,
			float _AttackRange,
			float Critical,
			float Burn,
			float Freezing,
			float Electric,
			EDebuff _Element,
			const FTransform& Transform
		);
	UFUNCTION(BlueprintCallable) //###########################
		static AMonster* SpawnByType(
			const AActor* WorldContextActor,
			TSubclassOf<AMonster> Type,
			FName _Name,
			float MaxHp,
			float Hp,
			float Speed,
			float AD,
			float Armor,
			float _AttackRange,
			float Critical,
			float Burn,
			float Freezing,
			float Electric,
			EDebuff _Element,
			const FTransform& Transform
		);
	UFUNCTION(BlueprintCallable)
		static AMonster* SpawnUsingDataTable(
			const AActor* WorldContextActor,
			FName id,
			const FTransform& Transform
		);


	UFUNCTION(BlueprintCallable)
		bool IsPlayerInSight();

	virtual void OnDie_Implementation() override;
	virtual void DieProduction();

	virtual bool ApplyDamageWithHitResult_Implementation(float Amount, float Critical, bool IsPlayHitMontage, const FHitResult& SweepResult) override;

	virtual void KnockBack(float Amount, FVector From, FVector Additional) override;

	UFUNCTION(BlueprintCallable)
		virtual void Pushed(AActor* OtherActor, UPrimitiveComponent* OtherComponent);
private:
	void DrawDebug();

	UFUNCTION()
		void OnHitBoxBeginOverlap(AControlledCharacter* OtherActor, const FHitResult& SweepResult);
};