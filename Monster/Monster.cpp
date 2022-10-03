// Fill out your copyright notice in the Description page of Project Settings.


//#include "AIController.h"
#include "Monster.h"

#include "PlayerCharacter.h"
#include "DefaultLevelScriptActor.h"
#include "AIController.h"
#include "MonsterAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "MonsterController.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "CharacterWidget.h"
#include "Engine/World.h"
#include "HitBoxComponent.h"
#include "Engine/EngineTypes.h"
#include "Math/NumericLimits.h"
#include "Engine/Level.h"
#include "NavigationSystem.h"
#include "CustomDataTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"


// Sets default values
AMonster::AMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AMonsterController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	//HealthWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	HealthWidgetComp->SetupAttachment(RootComponent);
	HealthWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	HealthWidgetComp->SetDrawAtDesiredSize(true);
	
	//HitBox = CreateDefaultSubobject<UHitBoxComponent>(TEXT("HitBox"));
	//
	//HitBox->SetIsCheckBeginOverlapOnce(true);
	//HitBox->DisableCollision();
	
	//HitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("AttackCollision"));
	//HitBox->SetupAttachment(GetMesh(), TEXT("AttackCollision"));
	//
	//HitBox->AttachToComponent(GetMesh())
	DebugOptions = 0;

	Name = FName("");
	Element = EDebuff::None;
	ExpForPlayer = 20.f;
}

void AMonster::PostInitializeComponents()
{
	Super::PostInitializeComponents();		
	
	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);		
	//HitBox->SetTargetToPlayer();
	
	//PossessedBy(NewObject<AMonsterController>());
}

void AMonster::OnSpawned_Implementation()
{
	PostInitializedValue();


	//HitBox->SetDamageInfo(this);

}

// Called when the game starts or when spawned
void AMonster::BeginPlay()
{
	Super::BeginPlay();

	//HitBox->Func_HitColliderBeginOverlap.AddDynamic(this, &AMonster::OnHitBoxBeginOverlap);
	for (auto HitBox : MeleeAttackHitBoxList)
	{
		HitBox->Func_HitColliderBeginOverlap.AddDynamic(this, &AMonster::OnHitBoxBeginOverlap);

		HitBox->DisableCollision();
		HitBox->SetDamageInfo(this);
		HitBox->SetTargetToPlayer();
		HitBox->SetIsCheckBeginOverlapOnce(true);
	}

	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerController(this, 0)->GetPawn());
	AIController = Cast<AMonsterController>(GetController());

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	if (HealthWidgetComp->GetWidgetClass() != nullptr)
	{
		UCharacterWidget* HPBar = Cast<UCharacterWidget>(HealthWidgetComp->GetUserWidgetObject());
		HPBar->SetOwner(this);
	}
	


	//AnimInstance = GetMesh()->GetAnimInstance();
}

// Called every frame
void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebug();	

	//GetWorld()->GetNavigationSystem()
	// 
	//FNavLocation temp;
	//if (FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())->ProjectPointToNavigation(GetActorLocation(), temp))
	//{
	//	SetActorHiddenInGame(false);
	//	Cast<UPrimitiveComponent>(GetRootComponent())->SetSimulatePhysics(true);
	//	SetActorEnableCollision(true);
	//}
	//else
	//{
	//	SetActorHiddenInGame(true);
	//	Cast<UPrimitiveComponent>(GetRootComponent())->SetSimulatePhysics(false);
	//	SetActorEnableCollision(false);
	//}

	if (IsValid(AnimInstance))
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AIController->StopMovement();
		}
	}

	if (IsDie)
	{
		DestroyTimer -= DeltaTime;
		if (DestroyTimer < 0.f)
		{
			Destroy();
		}
	}

	if (GetCharacterMovement()->IsFlying())
	{
		GetCharacterMovement()->StopActiveMovement();
	}

	if (GetActorLocation().Z < -2000.f)
	{
		Destroy();
	}
}

float AMonster::TakeDamage(
	float DamageAmount, 
	FDamageEvent const& DamageEvent, 
	AController* EventInstigator, 
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	AnimInstance->Montage_Play(HitMontage);

	return 0.0f;
}

// Called to bind functionality to input
void AMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMonster::SetActive(bool b)
{
	Active = b;

	SetActorHiddenInGame(!b);
	SetActorEnableCollision(b);
	SetActorTickEnabled(b);

	if (IsValid(AIController))
	{
		AIController->SetActive(b);
	}
}

bool AMonster::GetActive()
{
	return Active;
}

void AMonster::Attack_Implementation()
{
	for (auto HitBox : MeleeAttackHitBoxList)
	{
		HitBox->EmptyOverlappedActors();
	}

	if (IsValid(AttackMontage) && CanAttack) 
	{
		AnimInstance->Montage_Play(AttackMontage, 1.0f);		
	}
	else if(!IsValid(AttackMontage))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AttackMontage is not valid"));
	}
}

void AMonster::MoveToPlayer()
{	
	if (IsValid(Player) && IsValid(AIController) && CanMove)
	{
		AIController->MoveToActor(Player);		
	}
	else if(!IsValid(Player) || !IsValid(AIController))
	{
		if (!IsValid(Player))
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AMonster::MoveToPlayer(): Player value is not set"));
		if(!IsValid(AIController))
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AMonster::MoveToPlayer(): AIController value is not set"));
	}
}

void AMonster::MoveToLocation(FVector location)
{
	if (IsValid(AIController) && CanMove)
	{
		AIController->MoveToLocation(location);
	}
	else if (!IsValid(AIController))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AMonster::MoveToLocation(): Player value is not set"));
	}
		
}

void AMonster::TurnToMoveDirection(float speed)
{	
	if (CanMove && GetVelocity().Size() > 0.f)
	{
		FVector location = GetActorLocation();
		FRotator lookAtRotator = UKismetMathLibrary::FindLookAtRotation(location, location + GetVelocity());
		//SetActorRotation(UKismetMathLibrary::RLerp(GetActorRotation(), lookAtRotator, GetWorld()->GetDeltaSeconds() * speed, true));
		SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(), lookAtRotator, GetWorld()->DeltaTimeSeconds, speed));

		SetActorRotation(FRotator(0, GetActorRotation().Yaw, 0));
	}
}

void AMonster::TurnToPlayer(float Speed)
{
	if (IsValid(Player))
	{
		FVector location = GetActorLocation() * FVector(1.f, 1.f, 0.f);
		FVector playerLocation = Player->GetActorLocation() * FVector(1.f, 1.f, 0.f);
		FRotator lookAtRotator = UKismetMathLibrary::FindLookAtRotation(location, playerLocation);
		FMath::FInterpTo(0, 2, 3, 2);

		//InterpTo 함수로 바꾸는거 고려 
		//SetActorRotation(UKismetMathLibrary::RLerp(GetActorRotation(), lookAtRotator, GetWorld()->GetDeltaSeconds() * Speed, true));
		SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(), lookAtRotator, GetWorld()->DeltaTimeSeconds, Speed));

		SetActorRotation(FRotator(0, GetActorRotation().Yaw, 0));
	}
}

void AMonster::SetRotationToPlayerDirection()
{
	if (IsValid(Player))
	{
		FVector location = GetActorLocation() * FVector(1.f, 1.f, 0.f);
		FVector playerLocation = Player->GetActorLocation() * FVector(1.f, 1.f, 0.f);
		FRotator lookAtRotator = UKismetMathLibrary::FindLookAtRotation(location, playerLocation);

		SetActorRotation(lookAtRotator);
	}
}

float AMonster::DotProductToPlayer()
{
	if (!IsValid(Player)) {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AMonster::GetAngleToPlayer(): Player is not valid"));
		return 0.0f;
	}

	FVector forward = GetActorForwardVector();
	FVector dirToPlayer = Player->GetActorLocation() - GetActorLocation();
	dirToPlayer.Normalize();
	return FVector::DotProduct(forward, dirToPlayer);
}

float AMonster::DistToPlayer()
{
	if (!IsValid(Player))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AMonster::DistToPlayer(): Player is null"));
		return TNumericLimits<float>::Max();
	}

	return FVector::Dist(GetActorLocation(), Player->GetActorLocation());
}

float AMonster::GetAttackRange()
{
	return AttackRange;
}

APlayerCharacter* AMonster::GetPlayer()
{
	return Player;
}

EDebuff AMonster::GetElement()
{
	return Element;
}

void AMonster::DisableAttackCollision()
{
	for (auto HitBox : MeleeAttackHitBoxList)
	{
		HitBox->DisableCollision();
		HitBox->EmptyOverlappedActors();
	}
}

void AMonster::EnableAttackCollision()
{
	for (auto HitBox : MeleeAttackHitBoxList) 
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0, FColor::Cyan, TEXT("Attack Collision"));
		HitBox->EnableCollision();
	}
}

AMonster* AMonster::Spawn(
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
)
{
	UWorld* World;

	ADefaultLevelScriptActor* levelActor = Cast<ADefaultLevelScriptActor>(WorldContextActor->GetWorld()->GetLevelScriptActor());

	if (WorldContextActor == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AMonster::Spawn(): WorldContextActor is null"));
		return nullptr;
	}
	else
	{
		World = WorldContextActor->GetWorld();
	}

	if (levelActor->GetMonsterClassById(*Id) == TSubclassOf<AMonster>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AMonster::Spawn(): Incorrect id value"));
		return nullptr;
	}

	AMonster* Monster = World->SpawnActorDeferred<AMonster>(
		levelActor->GetMonsterClassById(*Id),
		Transform,
		nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding
		);

	if (Monster == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AMonster::Spawn(): Fail to spawn Monster"));
		return nullptr;
	}

	UGameplayStatics::FinishSpawningActor(
		Monster,
		Transform
	);

	Monster->AttackRange = _AttackRange;
	Monster->SetStatus(EStatus::MaxHP, MaxHp);
	Monster->SetStatus(EStatus::HP, Hp);
	Monster->SetStatus(EStatus::Speed, Speed);
	Monster->SetStatus(EStatus::AD, AD);
	Monster->SetStatus(EStatus::Armor, Armor);
	Monster->SetStatus(EStatus::Critical, Critical);
	Monster->SetStatus(EStatus::Burn, Burn);
	Monster->SetStatus(EStatus::Freezing, Freezing);
	Monster->SetStatus(EStatus::Electric, Electric);

	Monster->Name = _Name;
	Monster->Element = _Element;

	//Monster->GetMesh()->SetSkeletalMesh(levelActor->Assets[Id].Mesh);
	//Monster->AttackMontage = levelActor->Assets[Id].AttackMontage;
	//Monster->GetMesh()->SetAnimInstanceClass(levelActor->Assets[Id].AnimClass);

	Monster->Tags.Add(FName(_Name));

	Monster->OnSpawned();

	return Monster;
}

AMonster* AMonster::SpawnByType(const AActor* WorldContextActor, TSubclassOf<AMonster> Type, FName _Name, float MaxHp, float Hp, float Speed, float AD, float Armor, float _AttackRange, float Critical, float Burn, float Freezing, float Electric, EDebuff _Element, const FTransform& Transform)
{
	UWorld* World;

	//ADefaultLevelScriptActor* levelActor = Cast<ADefaultLevelScriptActor>(WorldContextActor->GetWorld()->GetLevelScriptActor());

	if (WorldContextActor == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AMonster::Spawn(): WorldContextActor is null"));
		return nullptr;
	}
	else
	{
		World = WorldContextActor->GetWorld();
	}

	AMonster* Monster = World->SpawnActorDeferred<AMonster>(
		Type,
		Transform,
		nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn//ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding
		);

	if (Monster == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AMonster::Spawn(): Fail to spawn Monster"));
		return nullptr;
	}

	UGameplayStatics::FinishSpawningActor(
		Monster,
		Transform
	);

	Monster->AttackRange = _AttackRange;
	Monster->SetStatus(EStatus::MaxHP, MaxHp);
	Monster->SetStatus(EStatus::HP, Hp);
	Monster->SetStatus(EStatus::Speed, Speed);
	Monster->SetStatus(EStatus::AD, AD);
	Monster->SetStatus(EStatus::Armor, Armor);
	Monster->SetStatus(EStatus::Critical, Critical);
	Monster->SetStatus(EStatus::Burn, Burn);
	Monster->SetStatus(EStatus::Freezing, Freezing);
	Monster->SetStatus(EStatus::Electric, Electric);

	Monster->Tags.Add(FName(_Name));

	Monster->Element = _Element;

	//Monster->GetMesh()->SetSkeletalMesh(levelActor->Assets[Id].Mesh);
	//Monster->AttackMontage = levelActor->Assets[Id].AttackMontage;
	//Monster->GetMesh()->SetAnimInstanceClass(levelActor->Assets[Id].AnimClass);

	Monster->OnSpawned();

	return Monster;
}

AMonster* AMonster::SpawnUsingDataTable(const AActor* WorldContextActor, FName id, const FTransform& Transform)
{
	ADefaultLevelScriptActor* levelActor = Cast<ADefaultLevelScriptActor>(WorldContextActor->GetWorld()->GetLevelScriptActor());
	FMonsterData* status = levelActor->GetMonsterData(id);

	EDebuff debuff = EDebuff::None;

	if (status == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("%s is not registered id to DBTable"),*id.ToString()));
		return nullptr;
	}

	if (status->Element == TEXT("F"))
	{
		debuff = EDebuff::Burn;
	}
	else if (status->Element == TEXT("I"))
	{
		debuff = EDebuff::Freezing;
	}
	else if (status->Element == TEXT("E"))
	{
		debuff = EDebuff::Electric;
	
	}
	AMonster* Monster = AMonster::Spawn(
		WorldContextActor, 
		id.ToString(), 
		*status->Name,
		status->HP,
		status->HP, 
		status->Speed, 
		status->AD, 
		status->Armor,
		status->AttackRange,
		0.f, 
		status->Burn,
		status->Freezing,
		status->Electric,
		debuff, 
		Transform
	);
		
	float level = WorldContextActor->GetWorld()->GetFirstPlayerController()->GetPawn<APlayerCharacter>()->GetPlayerLevel();
	Monster->AddStatus(EStatus::Armor, level * 0.5f);

	Monster->ExpForPlayer = status->Exp + level;

	return Monster;
}

bool AMonster::IsPlayerInSight()
{
	if (DistToPlayer() <= AttackRange + 700.f)
	{
		return true;
	}
	return false;
}

void AMonster::OnDie_Implementation()
{
	Super::OnDie_Implementation();

	//IsDie = true;
		
	if (DeathEffect != nullptr && IsValid(DeathEffect))
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			DeathEffect,
			GetActorLocation() + FVector(0.f, 0.f, -50.f),
			GetActorRotation()
		);
	}

	if (SoulOfThisMonster.Num() > 0)
	{
		int DropSoulIndex = UKismetMathLibrary::RandomIntegerInRange(0, SoulOfThisMonster.Num() - 1);

		if (SoulDropChance >= UKismetMathLibrary::RandomFloatInRange(1.f, 100.f))
		{
			FVector SoulDropLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetMesh()->GetComponentLocation().Z + 40.f);

			if (IsValid(SoulOfThisMonster[DropSoulIndex]))
			{
				AActor* Soul = GetWorld()->SpawnActor<AActor>(SoulOfThisMonster[DropSoulIndex], SoulDropLocation, GetActorRotation());
				if (Tags[1].IsValid())
				{
					Soul->Tags.Add(Tags[1]);
				}
			}
		}
	}

	DieProduction();
	
	Func_OnDie.Broadcast();

	//Destroy();
	//SetActive(false);
}

void AMonster::DieProduction()
{
	Cast<UMonsterAnimInstance>(AnimInstance)->SetIsDie(true);

	HealthWidgetComp->SetVisibility(false); //이거 안됨

	AnimInstance->Montage_Stop(0.5f);
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);	
	AIController->UnPossess();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(MonsterCollisionChannel, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(PlayerCollisionChannel, ECollisionResponse::ECR_Ignore);	
}

bool AMonster::ApplyDamageWithHitResult_Implementation(float Amount, float Critical, bool IsPlayHitMontage, const FHitResult& SweepResult)
{
	bool b = Super::ApplyDamageWithHitResult_Implementation(Amount, Critical, IsPlayHitMontage, SweepResult);

	if (IsDie)
	{
		FVector dir = GetActorLocation() - SweepResult.Location;
		dir.Normalize();
		GetMesh()->AddImpulse(dir * FMath::FRandRange(3000.f, 4000.f), NAME_None, true);
	}

	return b;
}

void AMonster::KnockBack(float Amount, FVector From, FVector Additional)
{
	Super::KnockBack(Amount, From, Additional);
}

void AMonster::Pushed(AActor* OtherActor, UPrimitiveComponent* OtherComponent)
{
	if (IsCanPushed)
	{
		if (OtherComponent->GetCollisionObjectType() == MonsterCollisionChannel)
		{
			GetCharacterMovement()->AddForce((GetActorLocation() - OtherActor->GetActorLocation()) * 50000.f);
		}
	}

}

void AMonster::DrawDebug()
{
	if (DebugOptions && !(DebugOptions & (int32)EMonsterDebugOptions::None))
	{				
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("%d"), DebugOptions));
		if (DebugOptions & (int32)EMonsterDebugOptions::Status)
		{
			DrawDebugStatus();
		}
		if (DebugOptions & (int32)EMonsterDebugOptions::AttackRange)
		{		
			if (DistToPlayer() < AttackRange)
			{
				DrawDebugCircle(
					GetWorld(),
					FRotationMatrix::Make(FRotator::MakeFromEuler(FVector(90, 90, 90))) * GetTransform().ToMatrixNoScale(),
					AttackRange,
					40,
					FColor::Green,
					false,
					-1.0f,
					0,
					5,
					true
				);
			}
			else
			{
				DrawDebugCircle(
					GetWorld(),
					FRotationMatrix::Make(FRotator::MakeFromEuler(FVector(90, 90, 90))) * GetTransform().ToMatrixNoScale(),
					AttackRange,
					40,
					FColor::Red,
					false,
					-1.0f,
					0,
					5,
					true
				);
			}
		}
	}
}

void AMonster::OnHitBoxBeginOverlap(AControlledCharacter* OtherActor, const FHitResult& SweepResult)
{	
	OtherActor->ApplyDamage(GetStatusResultValue(EStatus::AD),0, false);
	OtherActor->ApplyDebuff(GetElement(), GetDebuffStatusResultValue(GetElement()));

	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("%d"), Element));
	
	if (HitEffects[Element] != nullptr)
	{
		UNiagaraComponent* effect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			HitEffects[Element],
			OtherActor->GetActorLocation(),
			GetActorRotation()
		);

	}
}
