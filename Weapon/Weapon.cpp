// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ControlledCharacter.h"
#include "HitBoxComponent.h"
#include "WeaponAction.h"
#include "WeaponPassive.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AWeapon::AWeapon()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;


    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    SceneComponent->SetupAttachment(RootComponent);
    //RootComponent = SceneComponent;

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    StaticMesh->SetupAttachment(SceneComponent);

    HitBox = CreateDefaultSubobject<UHitBoxComponent>(TEXT("HitBox"));
    HitBox->SetupAttachment(StaticMesh);
    HitBox->SetIsCheckBeginOverlapOnce(true);

    ParticleTrailComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleTrail"));
    ParticleTrailComponent->SetupAttachment(StaticMesh);

    ChangeWeaponCurve =
        LoadObject<UCurveFloat>(NULL, TEXT("/Game/Object/Weapon/C_ChangeWeapon.C_ChangeWeapon"), NULL, LOAD_None, NULL);

    IsEquipped = false;

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    
    TArray<UMaterialInterface*> Materials = StaticMesh->GetMaterials();
    int i = 0;
    for (auto Mat : Materials)
    {
        UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(Mat, this);
        DynMaterials.Add(DynMat);
        StaticMesh->SetMaterial(i, DynMat);
        i++;
    }

    //DynMaterial = UMaterialInstanceDynamic::Create(StaticMesh->GetMaterial(0), this);
    //StaticMesh->SetMaterial(0, DynMaterial);

    if(IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
        PlayerMesh = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetCharacter()->GetMesh();
    //PlayerMesh = GetWorld()->GetFirstPlayerController()->GetCharacter()->GetMesh();

    if (ChangeWeaponCurve != nullptr)
    {
        FOnTimelineFloat TimelineProgress;
        TimelineProgress.BindUFunction(this, FName("ChangeWeaponProgress"));
        ChangeWeaponTimeline.AddInterpFloat(ChangeWeaponCurve, TimelineProgress);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AWeapon::BeginPlay(): ChangeWeaponCurve is null"));
    }

}

void AWeapon::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    GetComponents<UWeaponPassive>(Passives);
    GetComponents<UWeaponAction>(Actions);
    
    //HitBox = Cast<UHitBoxComponent>(GetComponentByClass(UHitBoxComponent::StaticClass()));
    if (IsValid(HitBox))
    {
        HitBox->SetTargetToMonster();
        HitBox->DisableCollision();
        HitBox->SetUseHitResult(true);
        HitBox->Func_HitColliderBeginOverlap.AddDynamic(this, &AWeapon::OnHitCollisionBeginOverlap);
    }

    ParticleTrailComponent->Template = ParticleTrail;
}

void AWeapon::OnHitCollisionBeginOverlap(AControlledCharacter* OtherActor, const FHitResult& SweepResult)
{  
    bool IsCritical = 
    OtherActor->ApplyDamageWithHitResult(
        Player->GetStatusResultValue(EStatus::AD), 
        Player->GetStatusResultValue(EStatus::Critical),
        true,
        SweepResult
        );

    if (IsCritical)
    {
        Player->OnCriticalByMeleeAttack(OtherActor);
    }

    if (WeaponElement != EDebuff::None)
    {
        OtherActor->ApplyDebuff(
            WeaponElement,
            Player->GetDebuffStatusResultValue(WeaponElement)
        );
    }

    if (HitEffects[WeaponElement] != nullptr)
    {
        FVector HitEffectLocation = SweepResult.Location;

        if (SweepResult.Location == FVector::ZeroVector)
        {
            HitEffectLocation = HitBox->GetComponentLocation();
        }

        UNiagaraComponent* effect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            HitEffects[WeaponElement],
            HitEffectLocation,
            OtherActor->GetActorRotation()
        );
    }

    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            HitSound,
            OtherActor->GetActorLocation()
        );
    }
}


// Called every frame
void AWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    ChangeWeaponTimeline.TickTimeline(DeltaTime);

    for (auto& element : Passives)
    {
        element->Tick();
    }
}

void AWeapon::BeginTrail()
{
    ParticleTrailComponent->BeginTrails(BeginParticleTrailSocket, EndParticleTrailSocket, ETrailWidthMode::ETrailWidthMode_FromFirst, 1.0);
    if(NiagaraTrail != nullptr && IsValid(NiagaraTrail))
        NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraTrail, StaticMesh, NiagaraTrailSocket, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::KeepRelativeOffset, true, true);
}

void AWeapon::EndTrail()
{
    ParticleTrailComponent->EndTrails();

    if (NiagaraComponent != nullptr && IsValid(NiagaraComponent) && NiagaraComponent->IsActive())
    {        
        NiagaraComponent->Deactivate();
        NiagaraComponent = nullptr;
    }
}

void AWeapon::SetPlayer(APlayerCharacter* player)
{
    Player = player;
    PlayerMesh = player->GetMesh();
}

void AWeapon::SetSlime(ASlime* slime)
{
    this->Slime = slime;
}

void AWeapon::OnEnableWeapon_Implementation()
{
    IsEquipped = true;
    //ChangeWeaponTimeline.ReverseFromEnd();
    ChangeWeaponTimeline.Reverse();

    for (auto& element : Passives)
    {
        element->OnEquipped();
    }

    Player->AddStatusMulValue(EStatus::AD, WeaponADIncreaseRate);
}

void AWeapon::OnDisableWeapon_Implementation()
{
    DisableWeaponCollision();

    IsEquipped = false;
    //ChangeWeaponTimeline.PlayFromStart();    
    ChangeWeaponTimeline.Play();

    HitBox->EmptyOverlappedActors();
    for (auto& element : Passives)
    {
        element->OnUnequipped();
    }
    EndTrail();

    ExcuteWeaponActionReleased();

    float value = -1.f * WeaponADIncreaseRate;
    Player->AddStatusMulValue(EStatus::AD, value);
}

void AWeapon::ChangeWeaponProgress(float value)
{
    for (auto DynMat : DynMaterials)
    {
        DynMat->SetScalarParameterValue("Amount", value);
        DynMat->SetVectorParameterValue("Axis", StaticMesh->GetUpVector());
        DynMat->SetVectorParameterValue("Pivot", PlayerMesh->GetSocketLocation(FName("weapon")));
    }

    //DynMaterial->SetScalarParameterValue("Amount", value);
    //DynMaterial->SetVectorParameterValue("Axis", StaticMesh->GetUpVector());
    //DynMaterial->SetVectorParameterValue("Pivot", PlayerMesh->GetSocketLocation(FName("weapon")));
}

void AWeapon::SetEnableImmediately(bool b)
{
    if (b == true)
    {
        for (auto DynMat : DynMaterials)
        {
            DynMat->SetScalarParameterValue("Amount", 0.f);
        }

        //DynMaterial->SetScalarParameterValue("Amount", 0.f);
        ChangeWeaponTimeline.SetNewTime(0.f);
    }
    else
    {
        for (auto DynMat : DynMaterials)
        {
            DynMat->SetScalarParameterValue("Amount", 1.f);
        }

        //DynMaterial->SetScalarParameterValue("Amount", 1.f);
        ChangeWeaponTimeline.SetNewTime(1.f);
    }
    for (auto DynMat : DynMaterials)
    {
        DynMat->SetVectorParameterValue("Axis", GetActorUpVector());
        DynMat->SetVectorParameterValue("Pivot", PlayerMesh->GetSocketLocation(FName("weapon")));
    }
    
    //DynMaterial->SetVectorParameterValue("Axis", GetActorUpVector());
    //DynMaterial->SetVectorParameterValue("Pivot", PlayerMesh->GetSocketLocation(FName("weapon")));
    
    IsEquipped = b;
}

EWeaponType AWeapon::GetType()
{
    return Type;
}

EDebuff AWeapon::GetWeaponElement()
{
    return WeaponElement;
}

float AWeapon::GetWeaponActionCoolTime()
{
    float coolTime = 0.f;
    if (Actions.Num() != 0)
    {
        for (auto* action : Actions)
        {
            coolTime = FMath::Max(coolTime, action->GetCoolTime());
        }
        return Actions[0]->GetCoolTime();
    }
    return 0.f;
}

float AWeapon::GetWeaponActionTimer()
{
    float timer = 0.f;
    if (Actions.Num() != 0)
    {
        for (auto* action : Actions)
        {
            timer = FMath::Max(timer, action->GetCoolTime());
        }
        return Actions[0]->GetTimer();
    }
    return 0.f;
}

void AWeapon::ExcuteWeaponPassiveTrigger()
{    
    for (auto& element : Passives)
    {
        element->Trigger();
    }
}

void AWeapon::ExcuteWeaponActionTrigger()
{
    for (auto& element : Actions)
    {
        if (element->GetIsCooldown() == false)
        {
            element->ExcuteTrigger();
        }
    }
}

void AWeapon::ExcuteWeaponActionReleased()
{
    for (auto& element : Actions)
    {
        element->ExcuteReleased();
    }
}

void AWeapon::EnableWeaponCollision()
{
    if(IsValid(HitBox))
        HitBox->EnableCollision();

    OnEnableWeaponCollision();
}

void AWeapon::DisableWeaponCollision()
{
    if (IsValid(HitBox))
        HitBox->DisableCollision();

    OnDisableWeaponCollision();
}

void AWeapon::OnEnableWeaponCollision_Implementation()
{
}

void AWeapon::OnDisableWeaponCollision_Implementation()
{

}

void AWeapon::SetParticleTrail(UParticleSystem* _ParticleTrail)
{
    ParticleTrailComponent->Template = _ParticleTrail;
    ParticleTrail = _ParticleTrail;
}

UHitBoxComponent* AWeapon::GetHitBox()
{
    return HitBox;
}

