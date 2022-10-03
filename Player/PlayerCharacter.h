// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ControlledCharacter.h"

#include "PlayerCharacter.generated.h"

UCLASS()
class TEAMPROJECT_API APlayerCharacter : public AControlledCharacter
{
    GENERATED_BODY()

        static APlayerCharacter* Instance;

    //Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class UCameraComponent* FollowCamera;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class UHitBoxComponent* TargetSearchingArea;

    UPROPERTY(EditAnywhere, Category = UI)
        TSubclassOf<UUserWidget> PlayerUIWidgetClass;
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
        class UUserWidget* PlayerUIWidget;

    UPROPERTY(EditAnywhere, Category = UI)
        TSubclassOf<UUserWidget> LevelUpUIWidgetClass;
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
        class UUserWidget* LevelUpUIWidget;

    UPROPERTY(EditAnywhere, Category = UI)
        TSubclassOf<UUserWidget> GameOverUIWidgetClass;
    UPROPERTY()
        class UUserWidget* GameOverUIWidget;

    UPROPERTY(EditAnywhere)
        class ATileManager* TileManager;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
        class UCapsuleComponent* HitBox;


    float CriticalPauseTimer = 0.f;

private:
    AActor* Target;

    bool CanWeaponActionDuringAttack = false;
    bool SaveInputWeaponActionDuringAttack = false;
    bool SaveInputWeaponActionReleaseDuringAttack = false;
    bool IsStartWeaponAction = false;

protected:
    UPROPERTY(BlueprintReadOnly, Category = Status)
        TArray<float> IncreaseADByWeaponType;

    UPROPERTY(BlueprintReadOnly, Category = Camera)
        bool IsLockOn;

    UPROPERTY(BlueprintReadOnly, Category = Movement)
        struct FVector MoveDirection;

    UPROPERTY(BlueprintReadOnly)
        class ASlime* Slime;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
        class USoundBase* WeaponChangeSound;

    //TArray<class AWeapon*> WeaponSlot;
    //uint8 CurWeaponIdx;

    //콤보
    class UAnimInstance* PlayerAnimInstance;

    UPROPERTY()
        TArray<class UAnimMontage*> AnimList;
    UPROPERTY(EditAnywhere, Category = Animation)
        TArray<class  UAnimMontage*> SwordAnimList;
    UPROPERTY(EditAnywhere, Category = Animation)
        TArray<class  UAnimMontage*> WandAnimList;
    UPROPERTY(EditAnywhere, Category = Animation)
        TArray<class  UAnimMontage*> TwoHandSwordAnimList;

    UPROPERTY(EditAnywhere, Category = Animation)
        class UAnimMontage* KnockBackAnim;


    bool CanMove;
    bool CanAttack;
    bool IsAttacking;
    bool SaveCombo;
    uint8 AttackCount;

    //Player Level Up
    TArray<class UUserWidget*> LevelUpUIArr;



    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        uint8 PlayerLevel;
    UPROPERTY(BlueprintReadOnly)
        float PlayerExp;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        float PlayerExpToLevelUp;
    UPROPERTY(BlueprintReadOnly)
        int Gold;
    UPROPERTY(BlueprintReadOnly)
        uint8 AmountOfMonstersDefeated; //AddExpToPlayer()에서 실행

    UPROPERTY(VisibleAnywhere)
        bool IsInBossRoom;

public:
    // Sets default values for this character's properties
    APlayerCharacter();

    UFUNCTION(BlueprintCallable)
        void SetCameraAngle(float pitch);

protected:
    virtual void PostInitializeComponents() override;
    // Called when the game starts or when spawned
    void BeginPlay() override;

    void MoveForward(float value);
    void MoveRight(float value);


private:
    void Move();

    void PlayAttackMontage();

    void TargetSearch();

public:
    static APlayerCharacter* GetInstance();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable)
        bool ChangeWeapon(uint8 idx);                            //슬롯 내 무기 변경 
    UFUNCTION()
        void PlayWeaponChangeSound();
    UFUNCTION(BlueprintCallable)
        bool AddWeapon(TSubclassOf<class AWeapon> weaponType);     //무기 획득
    UFUNCTION(BlueprintCallable)
        void AddAltarWeapon(TSubclassOf<class AWeapon> weaponType);     //무기 획득
    UFUNCTION(BlueprintCallable)
        void SetSlime(ASlime* _slime);
    UFUNCTION(Blueprintpure)
        ASlime* GetSlime();
    void SwitchWeaponAnimList();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
        void Attack();
    void Attack_Implementation();

    UFUNCTION(BlueprintCallable)
        void SaveAttackCombo();
    UFUNCTION(BlueprintCallable)
        void ResetAttackCombo();

    UFUNCTION(Blueprintpure)
        AWeapon* GetEquippingWeapon();

    void WeaponAction();
    void WeaponActionReleased();

    UFUNCTION(BlueprintCallable)
        void SpawnMonster(TSubclassOf<class AMonster> MonsterClass);

    UFUNCTION(Blueprintpure)
        TArray<class AWeapon*> GetWeaponSlot();
    UFUNCTION(Blueprintpure)
        bool GetWeaponSlotIsEmpty();

    UFUNCTION(Blueprintpure)
        AActor* GetTarget();

    virtual void EnableAttackCollision() override;
    virtual void DisableAttackCollision() override;

    UFUNCTION(BlueprintCallable)
        void Movement(float DeltaTime);
    void FindMoveDirection();
    void SetPlayerDirection(float DeltaTime, float Speed);
    UFUNCTION(BlueprintCallable)
        void AddExpToPlayer(float Exp);
    void CountAmountOfMonsterDefeated();
    void OnLevelUp();
    UFUNCTION(BlueprintCallable)
        void AddGoldToPlayer(int Value);

    UFUNCTION(BlueprintCallable)
        void SetCanAttack(bool b);

    virtual float GetStatusResultValue(EStatus type) override;
    virtual void OnDie_Implementation() override;
    UFUNCTION(BlueprintCallable)
        void ShowGameOverUI();
    void SaveGame();

    void OnCriticalByMeleeAttack(AControlledCharacter* OtherActor);

    virtual void KnockBack(float Amount, FVector From, FVector Additional) override;
    UFUNCTION(BlueprintImplementableEvent)
        void OnKnockBack();

    UFUNCTION(Blueprintpure)
        bool GetIsInBossRoom();
    UFUNCTION(BlueprintCallable)
        void SetIsInBossRoom(bool b);

    uint8 GetPlayerLevel();

    UFUNCTION(Blueprintpure)
        UCapsuleComponent* GetHitBox();
};