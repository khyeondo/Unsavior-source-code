// Fill out your copyright notice in the Description page of Project Settings.


#include "HitBoxComponent.h"
#include "ControlledCharacter.h"
#include "Monster.h"
#include "PlayerCharacter.h"
#include "Weapon.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UHitBoxComponent::UHitBoxComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bHiddenInGame = true;

	SetGenerateOverlapEvents(true);
	SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	TargetType = TEXT("None");

	SetUseCCD(true);
	bMultiBodyOverlap = true;
	SetCanEverAffectNavigation(false);



	IsCheckBeginOverlapOnce = false;
	// ...
}

void UHitBoxComponent::PostLoad()
{
	Super::PostLoad();

}

// Called when the game starts
void UHitBoxComponent::BeginPlay()
{
	Super::BeginPlay();
	//OnComponentHit.AddDynamic(this, &UHitBoxComponent::OnHitColliderOverlapBegin);
	OnComponentBeginOverlap.AddDynamic(this, &UHitBoxComponent::OnHitColliderOverlapBegin);
	OnComponentEndOverlap.AddDynamic(this, &UHitBoxComponent::OnHitColliderEndOverlap);
}


// Called every frame
void UHitBoxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GetOverlappingActors(OverlappingActors);

	for (auto& element : OverlappingActors)
	{
		if (Func_HitColliderOverlapping.IsBound())
		{
			Func_HitColliderOverlapping.Broadcast(Cast<AControlledCharacter>(element), DeltaTime);
		}
	}

}

void UHitBoxComponent::OnHitColliderOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Func_HitColliderBeginOverlap.IsBound())
	{
		if (UseHitResult)
		{
			TArray<FHitResult> Results;

			TArray<TEnumAsByte<EObjectTypeQuery>> Types;

			//if(TargetType == TEXT("Monster"))
			Types.Add(UEngineTypes::ConvertToObjectType(MonsterCollisionChannel));
			//else if(TargetType == TEXT("Player"))
			Types.Add(UEngineTypes::ConvertToObjectType(PlayerCollisionChannel));

			FVector Direction = GetUpVector() * CapsuleHalfHeight / 2.f;


			UKismetSystemLibrary::SphereTraceMultiForObjects(
				this,
				GetComponentLocation() + Direction,
				GetComponentLocation() - Direction,
				GetScaledCapsuleRadius(),
				Types,
				false,
				TArray<AActor*>(),
				EDrawDebugTrace::ForDuration,
				Results,
				true,
				FLinearColor::Blue,
				FLinearColor::Red,
				0.f
			);

			if (!IsCheckBeginOverlapOnce)
			{
				for (auto result : Results)
				{
					if (result.Actor == OtherActor)
					{
						Func_HitColliderBeginOverlap.Broadcast(Cast<AControlledCharacter>(OtherActor), result);
						return;
					}
				}
				Func_HitColliderBeginOverlap.Broadcast(Cast<AControlledCharacter>(OtherActor), SweepResult);
			}
			else
			{
				if (!OverlappedActors.Contains(OtherActor))
				{
					OverlappedActors.Add(OtherActor);
					for (auto result : Results)
					{
						if (result.Actor == OtherActor)
						{
							Func_HitColliderBeginOverlap.Broadcast(Cast<AControlledCharacter>(OtherActor), result);
							return;
						}
					}
					Func_HitColliderBeginOverlap.Broadcast(Cast<AControlledCharacter>(OtherActor), SweepResult);
				}
			}
		}
		else
		{
			if (!IsCheckBeginOverlapOnce)
			{
				Func_HitColliderBeginOverlap.Broadcast(Cast<AControlledCharacter>(OtherActor), SweepResult);
			}
			else
			{
				if (!OverlappedActors.Contains(OtherActor))
				{
					OverlappedActors.Add(OtherActor);
					Func_HitColliderBeginOverlap.Broadcast(Cast<AControlledCharacter>(OtherActor), SweepResult);
				}
			}
		}
	}
}

void UHitBoxComponent::OnHitColliderEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Func_HitColliderEndOverlap.IsBound())
	{
		Func_HitColliderEndOverlap.Broadcast(Cast<AControlledCharacter>(OtherActor));
	}
}

void UHitBoxComponent::SetUseHitResult(bool b)
{
	UseHitResult = b;
}

void UHitBoxComponent::SetTargetToPlayer()
{
	SetCollisionResponseToChannel(PlayerCollisionChannel, ECollisionResponse::ECR_Overlap);
	SetCollisionResponseToChannel(MonsterCollisionChannel, ECollisionResponse::ECR_Ignore);

	TargetType = TEXT("Player");
}

void UHitBoxComponent::SetTargetToMonster()
{
	SetCollisionResponseToChannel(PlayerCollisionChannel, ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(MonsterCollisionChannel, ECollisionResponse::ECR_Overlap);

	TargetType = TEXT("Monster");
}

void UHitBoxComponent::SetDamageInfoCustom(float _Damage, FString _TargetType, AActor* _TargetActor, float _StatusCriticalValue, float _StatusDebuffValue, EDebuff _Debuff)
{
	Damage = _Damage;
	if (_TargetType == TEXT("Player"))
	{
		SetTargetToPlayer();
	}
	else if (_TargetType == TEXT("Monster"))
	{
		SetTargetToMonster();
	}
	TargetActor = _TargetActor;
	StatusCriticalValue = _StatusCriticalValue;
	StatusDebuffValue = _StatusDebuffValue;
	Debuff = _Debuff;

	UpdateBodySetup();

	IsSetDamage = true;
	EmptyOverlappedActors();
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void UHitBoxComponent::SetDamageInfo(AControlledCharacter* Spawner)
{

	bool isDisable = false;
	if (GetCollisionEnabled() == ECollisionEnabled::NoCollision)
	{
		isDisable = true;
	}

	SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Damage = Spawner->GetStatusResultValue(EStatus::AD);
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("%f"), Damage));
	StatusCriticalValue = Spawner->GetStatusResultValue(EStatus::Critical);

	AMonster* Monster = Cast<AMonster>(Spawner);
	APlayerCharacter* Player = Cast<APlayerCharacter>(Spawner);
	if (IsValid(Monster))
	{
		StatusDebuffValue = Monster->GetDebuffStatusResultValue(Monster->GetElement());
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("%f"), StatusDebuffValue));
		Debuff = Monster->GetElement();
		TargetActor = Monster->GetPlayer();
		SetTargetToPlayer();
	}
	else if (IsValid(Player))
	{
		StatusDebuffValue = Player->GetDebuffStatusResultValue(Player->GetEquippingWeapon()->GetWeaponElement());
		Debuff = Player->GetEquippingWeapon()->GetWeaponElement();
		TargetActor = Player->GetTarget();
		SetTargetToMonster();
	}
	else
	{
		if (Spawner->GetStatusResultValue(EStatus::Burn) != 0.f)
		{
			StatusDebuffValue = Spawner->GetStatusResultValue(EStatus::Burn);

			Debuff = EDebuff::Burn;
		}
		else if (Spawner->GetStatusResultValue(EStatus::Electric) != 0.f)
		{
			StatusDebuffValue = Spawner->GetStatusResultValue(EStatus::Electric);
			Debuff = EDebuff::Electric;
		}
		else if (Spawner->GetStatusResultValue(EStatus::Freezing) != 0.f)
		{
			StatusDebuffValue = Spawner->GetStatusResultValue(EStatus::Freezing);
			Debuff = EDebuff::Freezing;
		}
		else
		{
			StatusDebuffValue = 0.f;
			Debuff = EDebuff::None;
		}
	}

	UpdateBodySetup();

	IsSetDamage = true;
	EmptyOverlappedActors();

	if (!isDisable)
		SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Func_OnSpawnedByActor.Broadcast(Spawner);
}

void UHitBoxComponent::SetDamageInfoByHitBox(UHitBoxComponent* SpawnerHitBox)
{
	bool isDisable = false;
	if (GetCollisionEnabled() == ECollisionEnabled::NoCollision)
	{
		isDisable = true;
	}

	SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Damage = SpawnerHitBox->Damage;
	StatusCriticalValue = SpawnerHitBox->StatusCriticalValue;
	Debuff = SpawnerHitBox->Debuff;
	StatusDebuffValue = SpawnerHitBox->StatusDebuffValue;
	TargetActor = SpawnerHitBox->TargetActor;

	if (SpawnerHitBox->GetTargetType() == TEXT("Monster"))
	{
		SetTargetToMonster();
	}
	else
	{
		SetTargetToPlayer();
	}
	UpdateBodySetup();

	IsSetDamage = true;
	EmptyOverlappedActors();

	if (!isDisable)
		SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//Func_OnSetDamageInfo.Broadcast();
}

FString UHitBoxComponent::GetTargetType()
{
	return TargetType;
}

AActor* UHitBoxComponent::GetTargetActor()
{
	return TargetActor;
}

float UHitBoxComponent::GetDamage()
{
	return Damage;
}

float UHitBoxComponent::GetCriticalProbaility()
{
	return StatusCriticalValue;
}

float UHitBoxComponent::GetDebuffProbaility()
{
	return StatusDebuffValue;
}

EDebuff UHitBoxComponent::GetDebuff()
{
	return Debuff;
}

void UHitBoxComponent::EnableCollision()
{
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void UHitBoxComponent::DisableCollision()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EmptyOverlappedActors();
}

void UHitBoxComponent::EmptyOverlappedActors()
{
	OverlappedActors.Empty();
}

void UHitBoxComponent::SetIsCheckBeginOverlapOnce(bool b)
{
	IsCheckBeginOverlapOnce = b;
}