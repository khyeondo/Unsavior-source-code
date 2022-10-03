// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimeLineComponent.h"
#include "GameFramework/Actor.h"
#include "ControlledCharacter.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Sword = 0 UMETA(DisplayName = "Sword"),
    Wand = 1 UMETA(DisplayName = "Wand"),
    Shield = 2 UMETA(DisplayName = "Shield"),
    TwoHandSword = 3 UMETA(DisplayName = "TwoHandSword"),

    Count UMETA(DisplayName = "Count")
};

UCLASS()
class TEAMPROJECT_API AWeapon : public AActor
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere)
    class USceneComponent* SceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UStaticMeshComponent* StaticMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UHitBoxComponent* HitBox;

    class UParticleSystemComponent* ParticleTrailComponent;
    class UNiagaraComponent* NiagaraComponent = nullptr;

protected:
    UPROPERTY(VisibleAnywhere, Category = "Timeline")
    UCurveFloat* ChangeWeaponCurve = nullptr;
    FTimeline ChangeWeaponTimeline;

    //UPROPERTY(BlueprintReadOnly)
    //class UMaterialInstanceDynamic* DynMaterial;
    UPROPERTY(BlueprintReadOnly)
    TArray<class UMaterialInstanceDynamic*> DynMaterials;
    

    UPROPERTY(BlueprintReadOnly)
        class USkeletalMeshComponent* PlayerMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        EWeaponType Type;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        EDebuff WeaponElement;
    UPROPERTY(BlueprintReadOnly)
        bool IsEquipped;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UNiagaraSystem* NiagaraTrail;
    UPROPERTY(EditAnywhere)
    UParticleSystem* ParticleTrail;

    UPROPERTY(EditAnywhere)
    FName BeginParticleTrailSocket;
    UPROPERTY(EditAnywhere)
    FName EndParticleTrailSocket;
    UPROPERTY(EditAnywhere)
    FName NiagaraTrailSocket;       

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Particle)
    TMap<EDebuff, class UNiagaraSystem*> HitEffects;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
     class USoundBase* HitSound;

    UPROPERTY(BlueprintReadOnly)
    class APlayerCharacter* Player;
    UPROPERTY(BlueprintReadOnly)
    class ASlime* Slime;

    TArray<class UWeaponPassive*> Passives;
    TArray<class UWeaponAction*> Actions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WeaponADIncreaseRate = 0.f;

public:
    // Sets default values for this actor's properties
    AWeapon();    

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    void BeginTrail();
    void EndTrail();

    void SetPlayer(APlayerCharacter* Player);
    void SetSlime(ASlime* slime);

    //무기 장착, 해제 시 호출
    UFUNCTION(BlueprintNativeEvent)
    void OnEnableWeapon();
    void OnEnableWeapon_Implementation();
    UFUNCTION(BlueprintNativeEvent)
    void OnDisableWeapon();
    void OnDisableWeapon_Implementation();
    UFUNCTION()
    void ChangeWeaponProgress(float value);
    void SetEnableImmediately(bool b);

    UFUNCTION(Blueprintpure)
    EWeaponType GetType();

    UFUNCTION(Blueprintpure)
    EDebuff GetWeaponElement();

    UFUNCTION(Blueprintpure)
    float GetWeaponActionCoolTime();
    UFUNCTION(Blueprintpure)
    float GetWeaponActionTimer();

    void ExcuteWeaponPassiveTrigger();
    void ExcuteWeaponActionTrigger();
    void ExcuteWeaponActionReleased();

    void EnableWeaponCollision();
    void DisableWeaponCollision();
    
    UFUNCTION(BlueprintNativeEvent)
    void OnEnableWeaponCollision();
    void OnEnableWeaponCollision_Implementation();
    UFUNCTION(BlueprintNativeEvent)
    void OnDisableWeaponCollision();
    void OnDisableWeaponCollision_Implementation();

    UFUNCTION(BlueprintCallable)
    void SetParticleTrail(UParticleSystem* _ParticleTrail);


    UHitBoxComponent* GetHitBox();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void PostInitializeComponents() override;

private:
    UFUNCTION()
    void OnHitCollisionBeginOverlap(class AControlledCharacter* OtherActor, const FHitResult& SweepResult);
};
