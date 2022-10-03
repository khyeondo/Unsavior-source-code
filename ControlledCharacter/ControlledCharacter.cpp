// Fill out your copyright notice in the Description page of Project Settings.



#include "ControlledCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "ControlledCharacterAnimInstance.h"
#include "EliteMonster.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
AControlledCharacter::AControlledCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	IsDie = false;
	CanMove = true;
	CanAttack = true;

	for (size_t i = 0; i < (int)EStatus::Count; i++)
	{
		Status.Add((EStatus)i, 1.f);
		StatusAddValue.Add((EStatus)i, 0.f);
		StatusMulValue.Add((EStatus)i, 1.f);
		StatusResultValue.Add((EStatus)i, 0.f);		
	}

	for (size_t i = 0; i < (int)EDebuff::Count; i++)
	{
		HitEffects.Add((EDebuff)i, nullptr);
		DebuffEffects.Add((EDebuff)i, nullptr);
		DebuffSounds.Add((EDebuff)i, nullptr);
		CurrentDebuffEffect.Add((EDebuff)i, nullptr);
	}

	IsWalk = false;

	Status[EStatus::Speed] = 600;

	DebuffTime.Init(0.f, (int)EDebuff::Count);
	InitialDebuffTime.Init(0.f, (int)EDebuff::Count);

	TargetPoint = CreateDefaultSubobject<USceneComponent>(TEXT("TargetPoint"));
	TargetPoint->SetupAttachment(GetMesh());
	TargetPoint->ComponentTags.Add("TargetPoint");
}

void AControlledCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PostInitializedValue();
	
}

// Called when the game starts or when spawned
void AControlledCharacter::BeginPlay()
{
	Super::BeginPlay();
	Controller = GetController();

	
}
void AControlledCharacter::PostInitializedValue()
{
	InitialMoveSpeed = Status[EStatus::Speed];
	for (size_t i = 0; i < (int)EStatus::Count; i++)
	{
		OnChangeStatus((EStatus)i);		
	}
	AnimInstance = GetMesh()->GetAnimInstance();
}

void AControlledCharacter::DrawDebugStatus()
{
	DrawDebugString(
		GetWorld(), FVector::ZeroVector,
		FString::Printf(
			TEXT("\
                    MaxHp: %.1f (%.1f + %.1f)\n\
                    HP: %.1f (%.1f + %.1f)\n\
                    Speep: %.1f (%.1f + %.1f)\n\
                    Armor: %.1f (%.1f + %.1f)\n\
                    AD: %.1f (%.1f + %.1f)\n\
				"\
			),
			StatusResultValue[EStatus::MaxHP], Status[EStatus::MaxHP], (StatusResultValue[EStatus::MaxHP] - Status[EStatus::MaxHP]),
			StatusResultValue[EStatus::HP], Status[EStatus::HP], (StatusResultValue[EStatus::HP] - Status[EStatus::HP]),
			StatusResultValue[EStatus::Speed], Status[EStatus::Speed], (StatusResultValue[EStatus::Speed] - Status[EStatus::Speed]),
			StatusResultValue[EStatus::Armor], Status[EStatus::Armor], (StatusResultValue[EStatus::Armor] - Status[EStatus::Armor]),
			StatusResultValue[EStatus::AD], Status[EStatus::AD], (StatusResultValue[EStatus::AD] - Status[EStatus::AD])
		),
		this, FColor::Magenta, -0.1f, false, 1.f);
}

// Called every frame
void AControlledCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (size_t i = 0; i < (int)EDebuff::Count; i++)
	{
		float preTime = DebuffTime[i];

		DebuffTime[i] -= DeltaTime;
		DebuffTime[i] = fmax(0.f, DebuffTime[i]);

		if (preTime > 0.f && DebuffTime[i] <= 0.f)
		{
			OnEndDebuff((EDebuff)i);
		}
		else if (DebuffTime[i] > 0.f)
		{
			OnTickDebuff((EDebuff)i, DeltaTime);
		}
	}

	if (GetCharacterMovement()->IsFalling())
	{
		if (GetActorLocation().Z > 200.f + GroundHeight)
		{
			LaunchCharacter(FVector(0.f, 0.f, (200.f - GetActorLocation().Z) * DeltaTime * 100.f), true, true);
		}
	}
	else
	{
		GroundHeight = GetActorLocation().Z;
	}

	//if (GetActorLocation().Z > 200.f)
	//{
	//	LaunchCharacter(FVector(0.f,0.f, (200.f-GetActorLocation().Z)*DeltaTime *100.f), true, true);
	//}
}

void AControlledCharacter::SubHP_Implementation(float f)
{
	if (!IsDie)
	{
		Status[EStatus::HP] -= f;
		if (Status[EStatus::HP] <= 0.f)
		{
			Status[EStatus::HP] = 0.f;
			OnDie();
		}
	}
}

void AControlledCharacter::OnDie_Implementation()
{
	IsDie = true;

	//for (size_t i = 0; i < (int)EDebuff::Count; i++)
	//{
	//	if (IsValid(CurrentDebuffEffect[(EDebuff)i]) && CurrentDebuffEffect[(EDebuff)i] != nullptr)
	//	{
	//		CurrentDebuffEffect[(EDebuff)i]->DestroyComponent();		
	//		//CurrentDebuffEffect[(EDebuff)i]->dest
	//	}
	//}
}

void AControlledCharacter::SetDebuffTime(EDebuff debuffType, float time)
{
	float preTime = DebuffTime[(int)debuffType];

	DebuffTime[(int)debuffType] = fmax(0.f, time);
	InitialDebuffTime[(int)debuffType] = time;

	if (preTime == 0.f && DebuffTime[(int)debuffType] > 0.f)
	{
		OnBeginDebuff(debuffType);
	}
}

void AControlledCharacter::AddDebuffTime(EDebuff debuffType, float time)
{
	float preTime = DebuffTime[(int)debuffType];

	DebuffTime[(int)debuffType] += time;
	DebuffTime[(int)debuffType] = fmax(0.f, DebuffTime[(int)debuffType]);

	if (preTime == 0.f && DebuffTime[(int)debuffType] > 0.f)
	{
		OnBeginDebuff(debuffType);
	}
}

void AControlledCharacter::OnBeginDebuff_Implementation(EDebuff debuffType)
{
	switch (debuffType)
	{
	case EDebuff::Burn:
		BeginBurning();
		break;

	case EDebuff::Freezing:
		BeginFreezing();
		break;

	case EDebuff::Electric:
		BeginElectrocuting();
		break;
	default:
		break;
	}

	//Debuff Effects
	if (DebuffEffects[debuffType] != nullptr)
	{
		CurrentDebuffEffect[debuffType] = UGameplayStatics::SpawnEmitterAttached(
			DebuffEffects[debuffType],
			GetMesh(),
			TEXT(""),
			FVector(0.f ,0.f, 90.f)
		);
	}
	
	//Debuff Sounds
	if (DebuffSounds[debuffType] != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			DebuffSounds[debuffType],
			GetActorLocation()
		);
	}
}

void AControlledCharacter::OnTickDebuff_Implementation(EDebuff debuffType, float deltaTime)
{
	switch (debuffType)
	{
	case EDebuff::Burn:
		Burning(deltaTime);
		break;

	case EDebuff::Freezing:
		Freezing(deltaTime);
		break;

	case EDebuff::Electric:
		Electrocuting(deltaTime);
		break;
	default:
		break;
	}
}

void AControlledCharacter::OnEndDebuff_Implementation(EDebuff debuffType)
{
	switch (debuffType)
	{
	case EDebuff::Burn:
		EndBurning();
		break;

	case EDebuff::Freezing:
		EndFreezing();
		break;

	case EDebuff::Electric:
		EndElectrocuting();
		break;
	default:
		break;
	}

	if (CurrentDebuffEffect[debuffType] != nullptr)
	{
		CurrentDebuffEffect[debuffType]->DestroyComponent();
	}
}

void AControlledCharacter::SetStatus(EStatus type, float value)
{
	Status[type] = value;
	OnChangeStatus(type);
}

float AControlledCharacter::GetStatus(EStatus type)
{
	return Status[type];
}
float AControlledCharacter::GetStatusResultValue(EStatus type)
{
	return StatusResultValue[type];
}

void AControlledCharacter::AddStatus(EStatus type, float value)
{
	Status[type] += value;
	OnChangeStatus(type);
}

void AControlledCharacter::AddStatusAddValue(EStatus type, float value)
{
	StatusAddValue[type] += value;
	OnChangeStatus(type);
}

void AControlledCharacter::AddStatusMulValue(EStatus type, float value)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f,FColor::Red, FString::Printf(TEXT("%f"), value));
	StatusMulValue[type] += value;
	OnChangeStatus(type);
}

void AControlledCharacter::OnChangeStatus_Implementation(EStatus type)
{
	StatusResultValue[type] = (Status[type] + StatusAddValue[type]) * StatusMulValue[type];

	switch (type)
	{
	case EStatus::MaxHP:
		break;

	case EStatus::HP:
		Status[EStatus::HP] = fmin(Status[EStatus::HP], Status[EStatus::MaxHP]);
		break;

	case EStatus::Speed:
		if(IsWalk)
			GetCharacterMovement()->MaxWalkSpeed = StatusResultValue[EStatus::Speed] / 3.f;
		else
			GetCharacterMovement()->MaxWalkSpeed = StatusResultValue[EStatus::Speed];
		RunAnimationPlayRate = (float)StatusResultValue[EStatus::Speed] / InitialMoveSpeed;
		break;

	case EStatus::Armor:		
		break;

	case EStatus::AD:
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("%f"), Status[type]));
		break;

	default:
		break;
	}
}

float AControlledCharacter::GetStatusMulValue(EStatus type)
{
	return StatusMulValue[type];
}

bool AControlledCharacter::GetIsWalk()
{
	return IsWalk;
}

float AControlledCharacter::GetDebuffStatusResultValue(EDebuff Debuff)
{
	switch (Debuff)
	{
	case EDebuff::Burn:
		return StatusResultValue[EStatus::Burn];
		break;
	case EDebuff::Freezing:
		return StatusResultValue[EStatus::Freezing];
		break;
	case EDebuff::Electric:
		return StatusResultValue[EStatus::Electric];
		break;
	}

	return 0.f;
}

bool AControlledCharacter::GetCanMove()
{
	return CanMove;
}

void AControlledCharacter::DisableAttackCollision()
{	
}
void AControlledCharacter::EnableAttackCollision()
{	
}

//-------------On Begin Debuff
void AControlledCharacter::BeginBurning()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Begin Burning"));
}

void AControlledCharacter::BeginFreezing()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Begin Freezing"));

	AddStatusMulValue(EStatus::Speed, -0.5f);
}

void AControlledCharacter::BeginElectrocuting()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Begin Electrocuting"));
	CanAttack = false;
	CanMove = false;
}

void AControlledCharacter::BeginBleeding()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Begin Bleeding"));
}

void AControlledCharacter::BeginFractured()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Begin Fractured"));
}

//-------------Tick Debuff
void AControlledCharacter::Burning(float deltaTime)
{
	AControlledCharacter* self =  Cast<AEliteMonster>(this);
	
	if (self == nullptr)//������ �ƴ� ���
	{
		self = Cast<AMonster>(this);
		if (self != nullptr)
		{
			SubHP(GetStatusResultValue(EStatus::MaxHP) / 20.f * deltaTime);
			return;
		}
	}
	SubHP(GetStatusResultValue(EStatus::MaxHP) / 100.f * deltaTime);
}

void AControlledCharacter::Freezing(float deltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Blue, TEXT("Tick Freezing"));
}

void AControlledCharacter::Electrocuting(float deltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Blue, TEXT("Tick Electrocuting"));
}

void AControlledCharacter::Bleeding(float deltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Blue, TEXT("Tick Bleeding"));
}

void AControlledCharacter::Fractured(float deltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Blue, TEXT("Tick Fractured"));
}

//-------------End Debuff
void AControlledCharacter::EndBurning()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("End Burning"));
}

void AControlledCharacter::EndFreezing()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("End Freezing"));

	AddStatusMulValue(EStatus::Speed, 0.5f);
}

void AControlledCharacter::EndElectrocuting()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("End Electrocuting"));
	CanAttack = true;
	CanMove = true;
}

void AControlledCharacter::EndBleeding()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("End Bleeding"));
}

void AControlledCharacter::EndFractured()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("End Fractured"));
}

bool AControlledCharacter::ApplyDamage_Implementation(float Amount, float Critical, bool IsPlayHitMontage)
{	
	OnHit();

	float Armor = 1.f-(StatusResultValue[EStatus::Armor]/(20.f + StatusResultValue[EStatus::Armor]));

	if (IsPlayHitMontage && IsValid(HitMontage))
	{
		AnimInstance->Montage_Play(HitMontage);
	}
	if (GettingHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GettingHitSound, GetActorLocation());
	}

	float RandomValue = FMath::RandRange(0.f, 1.f);
	
	float CriticalRate = 1.f - FMath::Pow(0.05f, Critical / 50.f);

	if (RandomValue < CriticalRate)
	{

		SubHP(fmax(1.f, Amount * 1.5f* Armor));
		return true;
	}
	else
	{
		SubHP(fmax(1.f, Amount* Armor));
		return false;
	}
}

bool AControlledCharacter::ApplyDamageWithHitResult_Implementation(float Amount, float Critical, bool IsPlayHitMontage, const FHitResult& SweepResult)
{
	return ApplyDamage_Implementation(Amount, Critical, IsPlayHitMontage);
}

void AControlledCharacter::OnHit_Implementation()
{
}

void AControlledCharacter::KnockBack(float Amount, FVector From, FVector Additional)
{
	FVector dir; 
	dir = GetActorLocation() * FVector(1,1,0) - From * FVector(1,1,0);
	dir.Normalize();
	dir *= Amount;

	LaunchCharacter(dir + Additional, false, false);
}

void AControlledCharacter::ApplyDebuff(EDebuff Debuff, float StatusValue)
{
	if (!IsDie)
	{
		float Probability = 1.f - FMath::Pow(0.5f, StatusValue / 20.f);
		float RandValue = FMath::FRand();

		if (RandValue < Probability)
		{
			switch (Debuff)
			{
			case EDebuff::Burn:
				SetDebuffTime(EDebuff::Burn, 5.f);
				break;
			case EDebuff::Freezing:
				SetDebuffTime(EDebuff::Freezing, 3.f);
				break;
			case EDebuff::Electric:
				SetDebuffTime(EDebuff::Electric, 2.f);
				break;
			}
		}
	}
}
void AControlledCharacter::SetIsWalk(bool b)
{
	IsWalk = b;
	OnChangeStatus(EStatus::Speed);
}

bool AControlledCharacter::GetIsDie()
{
	return IsDie;
}
