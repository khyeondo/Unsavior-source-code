// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Math/Vector.h"
#include "Weapon.h"
#include "Slime.h"
#include "WeaponAction.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.h"
#include "Monster.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "CharacterWidget.h"
#include "HitBoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "RoguelikeSaveGame.h"
#include "DefaultLevelScriptActor.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "BGMChanger.h"
#include "TileManager.h"

#include "EliteMonster.h"

APlayerCharacter* APlayerCharacter::Instance = nullptr;

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller	

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	TargetSearchingArea = CreateDefaultSubobject<UHitBoxComponent>(TEXT("TargetSearchingArea"));
	TargetSearchingArea->SetTargetToMonster();
	TargetSearchingArea->EnableCollision();
	TargetSearchingArea->SetupAttachment(RootComponent);

	HitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitBox"));

	Slime = nullptr;
	Target = nullptr;

	IsLockOn = false;

	//WeaponSlot.Init(nullptr, 4);

	AnimList.Init(nullptr, 20);
	CanMove = true;
	IsAttacking = false;
	CanAttack = true;
	SaveCombo = false;
	AttackCount = 0;
	//Player Leveling

	PlayerLevel = 1;
	PlayerExp = 0.f;
	PlayerExpToLevelUp = 10 * UKismetMathLibrary::MultiplyMultiply_FloatFloat((UKismetMathLibrary::Log((100 * PlayerLevel), 10.f)), 4) - 60;
	Gold = 0;
	AmountOfMonstersDefeated = 0;

	for (size_t i = 0; i < (int)EWeaponType::Count; i++)
	{
		IncreaseADByWeaponType.Add(1);
	}
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	HitBox->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	PlayerAnimInstance = GetMesh()->GetAnimInstance();
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//Set camera
	SetCameraAngle(-60.f);
	CameraBoom->TargetArmLength = 1400.f;

	//Player Levelling
	PlayerLevel = 1;
	PlayerExp = 0.f;
	PlayerExpToLevelUp = 10 * UKismetMathLibrary::MultiplyMultiply_FloatFloat((UKismetMathLibrary::Log((100 * PlayerLevel), 10.f)), 4) - 60;
	Gold = 0;
	AmountOfMonstersDefeated = 0;

	if (IsValid(GetEquippingWeapon()))
	{
		SwitchWeaponAnimList();
	}

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	//Player Widget
	if (IsValid(PlayerUIWidgetClass))
	{
		PlayerUIWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerUIWidgetClass);
		if (IsValid(PlayerUIWidget))
		{
			PlayerUIWidget->AddToViewport();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("PlayerWidgetClass is not valid"));
	}

	if (IsValid(LevelUpUIWidgetClass))
	{
		LevelUpUIWidget = CreateWidget<UUserWidget>(GetWorld(), LevelUpUIWidgetClass);
	}

	//Load Save Data & Set Player Status
	if (UGameplayStatics::DoesSaveGameExist("MySaveGame", 0))
	{
		URoguelikeSaveGame* LoadGameInstance = Cast<URoguelikeSaveGame>(UGameplayStatics::CreateSaveGameObject(URoguelikeSaveGame::StaticClass()));

		if (LoadGameInstance)
		{
			LoadGameInstance->SaveSlotName = "MySaveGame";
			LoadGameInstance->SaveIndex = 0;

			LoadGameInstance = Cast<URoguelikeSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->SaveIndex));

			//Set Player Status
			SetStatus(EStatus::MaxHP, LoadGameInstance->UpgradeFactor[EUpgradeFactor::MaxHP]);
			SetStatus(EStatus::HP, GetStatus(EStatus::MaxHP));
			SetStatus(EStatus::Speed, LoadGameInstance->UpgradeFactor[EUpgradeFactor::Speed]);
			SetStatus(EStatus::Critical, LoadGameInstance->UpgradeFactor[EUpgradeFactor::Critical]);
			SetStatus(EStatus::Armor, LoadGameInstance->UpgradeFactor[EUpgradeFactor::Armor]);
			SetStatus(EStatus::AD, LoadGameInstance->UpgradeFactor[EUpgradeFactor::AD]);
			SetStatus(EStatus::ExpIncreaseRate, LoadGameInstance->UpgradeFactor[EUpgradeFactor::ExpIncreaseRate]);
			SetStatus(EStatus::Burn, LoadGameInstance->UpgradeFactor[EUpgradeFactor::Burn]);
			SetStatus(EStatus::Freezing, LoadGameInstance->UpgradeFactor[EUpgradeFactor::Freezing]);
			SetStatus(EStatus::Electric, LoadGameInstance->UpgradeFactor[EUpgradeFactor::Electric]);

			IncreaseADByWeaponType[0] = LoadGameInstance->UpgradeFactor[EUpgradeFactor::SwordAD];
			IncreaseADByWeaponType[1] = LoadGameInstance->UpgradeFactor[EUpgradeFactor::WandAD];
			IncreaseADByWeaponType[2] = LoadGameInstance->UpgradeFactor[EUpgradeFactor::ShieldAD];
			IncreaseADByWeaponType[3] = LoadGameInstance->UpgradeFactor[EUpgradeFactor::TwoHandAD];
		}
	}
	//Increase Level and Show UI
	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			if (0 < LevelUpUIArr.Num() && LevelUpUIArr.Num() < 50)
			{
				LevelUpUIArr[0]->AddToViewport();
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("Add To Viewport"));
				LevelUpUIArr.RemoveAt(0);
				PlayerLevel++;
			}
		}), 0.1f, true);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (CanMove && !AnimInstance->IsAnyMontagePlaying() && !IsDie)
	{
		Movement(DeltaTime);
	}

	TargetSearch();

	if (CriticalPauseTimer != 0.f)
		CriticalPauseTimer -= DeltaTime;

	if (CriticalPauseTimer <= 0.f)
	{
		AnimInstance->Montage_Resume(nullptr);
		CriticalPauseTimer = 0.f;
	}

	if (SaveInputWeaponActionReleaseDuringAttack && IsStartWeaponAction)
	{
		WeaponActionReleased();
		SaveInputWeaponActionReleaseDuringAttack = false;
	}

	if (IsValid(TileManager))
		TileManager->CustomTick(DeltaTime);

	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("MaxHP : ") + FString::SanitizeFloat(GetStatusResultValue(EStatus::MaxHP)));
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Speed : ") + FString::SanitizeFloat(GetStatusResultValue(EStatus::Speed)));
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Critical : ") + FString::SanitizeFloat(GetStatusResultValue(EStatus::Critical)));
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Armor : ") + FString::SanitizeFloat(GetStatusResultValue(EStatus::Armor)));
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("ExpIncrease : ") + FString::SanitizeFloat(GetStatusResultValue(EStatus::ExpIncreaseRate)));
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Burn Chance : ") + FString::SanitizeFloat(GetStatusResultValue(EStatus::Burn)));
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Freezing Chance : ") + FString::SanitizeFloat(GetStatusResultValue(EStatus::Freezing)));
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Electric Chance : ") + FString::SanitizeFloat(GetStatusResultValue(EStatus::Electric)));
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Player Exp : ") + FString::SanitizeFloat(PlayerExp));
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Player AD Result Value : ") + FString::SanitizeFloat(GetStatusResultValue(EStatus::AD)));
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Increase AD By Weapon Type : ") + FString::SanitizeFloat(IncreaseADByWeaponType[(int)GetEquippingWeapon()->GetType()]));
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::Attack);
	PlayerInputComponent->BindAction("WeaponAction", IE_Pressed, this, &APlayerCharacter::WeaponAction);
	PlayerInputComponent->BindAction("WeaponAction", IE_Released, this, &APlayerCharacter::WeaponActionReleased);
}

void APlayerCharacter::SetCameraAngle(float pitch)
{
	FRotator controlRotation = GetControlRotation();
	controlRotation.Pitch = pitch;
	GetController()->SetControlRotation(controlRotation);
}

void APlayerCharacter::MoveForward(float value)
{

}

void APlayerCharacter::MoveRight(float value)
{

}

void APlayerCharacter::Movement(float DeltaTime)
{
	FindMoveDirection(); //플레이어 이동방향 결정
	SetPlayerDirection(DeltaTime, 10.f); //플레이어가 바라보는 방향 결정  
	Move();
}

void APlayerCharacter::FindMoveDirection()
{
	//Axis입력값으로 이동방향 결정 

	float forwardValue = GetInputAxisValue("MoveForward");
	float rightValue = GetInputAxisValue("MoveRight");

	FRotator rotator = FRotator(0.f, GetControlRotation().Yaw, 0.f);

	FVector forwardVector = FVector::ForwardVector;
	FVector rightVector = FVector::RightVector;

	forwardVector = rotator.RotateVector(forwardVector);
	rightVector = rotator.RotateVector(rightVector);

	forwardVector *= forwardValue;
	rightVector *= rightValue;

	MoveDirection = forwardVector + rightVector;

	MoveDirection.Normalize();
}

void APlayerCharacter::SetPlayerDirection(float DeltaTime, float Speed)
{
	//lockon값에 따라 캐릭터가 바라보는 방향 결정 
	if (!IsLockOn)
	{
		if (GetInputAxisValue("MoveForward") != 0.f || GetInputAxisValue("MoveRight") != 0.f)
		{
			FVector location = GetActorLocation();
			FRotator lookAtRotator = UKismetMathLibrary::FindLookAtRotation(location, location + MoveDirection);

			SetActorRotation(UKismetMathLibrary::RLerp(GetActorRotation(), lookAtRotator, DeltaTime * Speed, true));
		}
	}
	else
	{
	}
}

void APlayerCharacter::Move()
{

	if (GetInputAxisValue("MoveForward") != 0.f || GetInputAxisValue("MoveRight") != 0.f)
	{
		//패드 사용시 axis값 이동속도에 적용
		float axisValueScale = FVector2D(GetInputAxisValue("MoveForward"), GetInputAxisValue("MoveRight")).Size();

		AddMovementInput(MoveDirection, axisValueScale, false);
	}
}

bool APlayerCharacter::ChangeWeapon(uint8 idx)
{
	if (!IsValid(Slime))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("APlayerCharacter::ChangeWeapon(): Slime is not valid"));
		return false;
	}

	return Slime->ChangeWeapon(idx);
}

void APlayerCharacter::PlayWeaponChangeSound()
{
	if (WeaponChangeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponChangeSound, GetActorLocation());
	}
}

bool APlayerCharacter::AddWeapon(TSubclassOf<AWeapon> weaponType)
{
	if (!IsValid(Slime))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("APlayerCharacter::AddWeapon(): Slime is not valid"));
		return false;
	}
	return Slime->AddWeapon(weaponType);
}

void APlayerCharacter::AddAltarWeapon(TSubclassOf<class AWeapon> weaponType)
{
	if (!IsValid(Slime))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("APlayerCharacter::AddWeapon(): Slime is not valid"));
		return;
	}

	Slime->AddAltarWeapon(weaponType);
}

void APlayerCharacter::SetSlime(ASlime* _slime)
{
	this->Slime = _slime;
	Slime->AttachToPlayer(this);
}

void APlayerCharacter::Attack_Implementation()
{
	if (CanAttack)
	{
		CanMove = false;
		GetEquippingWeapon()->GetHitBox()->EmptyOverlappedActors();

		//공격 중인 경우에는 콤보 세이브
		if (IsAttacking)
		{
			SaveCombo = true;
		}
		//공격 중이 아닌 경우에는 공격 시작
		else
		{
			IsAttacking = true;

			PlayAttackMontage();
		}
	}
}

void APlayerCharacter::SaveAttackCombo()
{
	CanWeaponActionDuringAttack = true;

	if (SaveInputWeaponActionDuringAttack)
	{
		WeaponAction();
		return;
	}

	if (SaveCombo)
	{
		SaveCombo = false;

		if (AttackCount < AnimList.Num())
		{
			AttackCount++;
		}
		else
		{
			AttackCount = 0;
		}

		CanWeaponActionDuringAttack = false;
		PlayAttackMontage();
	}
}

void APlayerCharacter::ResetAttackCombo()
{
	AttackCount = 0;
	SaveCombo = false;
	IsAttacking = false;
	CanMove = true;
	CanWeaponActionDuringAttack = false;
}

void APlayerCharacter::PlayAttackMontage()
{
	if (IsValid(PlayerAnimInstance))
	{
		if (AnimList.IsValidIndex(0))
		{
			if (AttackCount < AnimList.Num())
			{
				PlayerAnimInstance->Montage_Play(AnimList[AttackCount], 1.f);
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("AnimList is not valid"));
		}
	}
}

void APlayerCharacter::TargetSearch()
{
	TArray<AActor*> OverlappingActors;
	TargetSearchingArea->GetOverlappingActors(OverlappingActors);


	float shortestDist = MAX_FLT;
	AActor* target = nullptr;
	for (auto& element : OverlappingActors)
	{


		FVector dir = element->GetActorLocation() - GetActorLocation();
		dir.Normalize();

		if (FVector::DotProduct(dir, GetActorForwardVector()) > 0.2f)
		{
			float dist = FVector::Dist2D(GetActorLocation(), element->GetActorLocation());
			if (dist < shortestDist)
			{
				target = element;
				shortestDist = dist;
			}
		}
	}
	Target = target;
}

APlayerCharacter* APlayerCharacter::GetInstance()
{
	return Instance;
}

ASlime* APlayerCharacter::GetSlime()
{
	return Slime;
}

void APlayerCharacter::SwitchWeaponAnimList()
{
	switch (GetEquippingWeapon()->GetType())
	{
	case EWeaponType::Sword:
		AnimList = SwordAnimList;
		return;
	case EWeaponType::Wand:
		AnimList = WandAnimList;
		return;
	case EWeaponType::Shield:
		//쉴드 애니메이션
		return;
	case EWeaponType::TwoHandSword:
		AnimList = TwoHandSwordAnimList;
		return;
	}
}

AWeapon* APlayerCharacter::GetEquippingWeapon()
{
	if (!IsValid(Slime))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("APlayerCharacter::GetEquippingWeapon(): Slime is not valid"));
		return nullptr;
	}

	return Slime->GetEquippingWeapon();
}

void APlayerCharacter::WeaponAction()
{
	if (IsValid(GetEquippingWeapon()))
	{
		if (CanWeaponActionDuringAttack)
		{
			GetEquippingWeapon()->ExcuteWeaponActionTrigger();
			IsStartWeaponAction = true;
			ResetAttackCombo();
			SaveInputWeaponActionDuringAttack = false;
		}
		else if (!IsAttacking && !AnimInstance->Montage_IsPlaying(nullptr))
		{
			GetEquippingWeapon()->ExcuteWeaponActionTrigger();
			IsStartWeaponAction = true;
		}
		else if (IsAttacking)
		{
			SaveInputWeaponActionDuringAttack = true;
		}
	}
}
void APlayerCharacter::WeaponActionReleased()
{
	if (IsAttacking)
	{
		SaveInputWeaponActionReleaseDuringAttack = true;
	}

	if (IsStartWeaponAction)
	{
		GetEquippingWeapon()->ExcuteWeaponActionReleased();
		IsStartWeaponAction = false;
	}
}

void APlayerCharacter::SpawnMonster(TSubclassOf<AMonster> MonsterClass)
{
	const FVector SpawnLocation = GetActorTransform().GetLocation() + GetActorForwardVector() * 500.f;
	const FRotator SpawnRotation = GetActorRotation();

	GetWorld()->SpawnActor<AMonster>(MonsterClass, SpawnLocation, SpawnRotation);
}

TArray<AWeapon*> APlayerCharacter::GetWeaponSlot()
{
	if (!IsValid(Slime))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("APlayerCharacter::GetWeaponSlot(): Slime is not valid"));
		return TArray<AWeapon*>();
	}

	return Slime->GetWeaponSlot();
}

bool APlayerCharacter::GetWeaponSlotIsEmpty()
{
	if (!IsValid(Slime))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("APlayerCharacter::GetWeaponSlotIsEmpty(): Slime is not valid"));
		return false;
	}

	return Slime->GetIsEmpty();
}
AActor* APlayerCharacter::GetTarget()
{
	return Target;
}

void APlayerCharacter::EnableAttackCollision()
{
	GetEquippingWeapon()->EnableWeaponCollision();
}

void APlayerCharacter::DisableAttackCollision()
{
	GetEquippingWeapon()->DisableWeaponCollision();
}

void APlayerCharacter::AddExpToPlayer(float Exp)
{

	if (!IsDie)
	{
		AmountOfMonstersDefeated++;

		PlayerExp += (Exp + GetStatusResultValue(EStatus::ExpIncreaseRate));
		while (PlayerExp >= PlayerExpToLevelUp)
		{
			//Level이 높아지는 코드는 AddToViewport와 함께 Beginplay()의 타이머에서 사용
			PlayerExp -= PlayerExpToLevelUp;
			OnLevelUp();
			PlayerExpToLevelUp = 10 * UKismetMathLibrary::MultiplyMultiply_FloatFloat((UKismetMathLibrary::Log((100 * (PlayerLevel + 1)), 10.f)), 4) - 60;
		}
	}
}



void APlayerCharacter::OnLevelUp()
{
	if (IsValid(LevelUpUIWidget))
	{
		LevelUpUIArr.Add(LevelUpUIWidget);
	}
	SetStatus(EStatus::HP, GetStatusResultValue(EStatus::MaxHP));
	//Cast<ADefaultLevelScriptActor>(GetWorld()->GetLevelScriptActor())->OnPlayerLevelUp(PlayerLevel);
}

void APlayerCharacter::AddGoldToPlayer(int Value)
{
	Gold += Value;
}

#pragma region 게임오버 시 처리

void APlayerCharacter::OnDie_Implementation()
{
	Super::OnDie_Implementation();

	DisableAttackCollision();	// 무기 콜리전 비활성화
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);	// 캐릭터 콜리전 비활성화
	HitBox->SetGenerateOverlapEvents(false);
	DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));	// 입력 비활성화

	// BGM Fade Out
	TArray<AActor*> BGMChanger_ref;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "BGMChanger", BGMChanger_ref);

	if (IsValid(BGMChanger_ref[0]))
	{
		Cast<ABGMChanger>(BGMChanger_ref[0])->FadeOutCurrentBGM(1.f);
	}

	AnimInstance->StopAllMontages(0.f);	// 사망 전 모든 애니메이션 정지
	Cast<UPlayerAnimInstance>(AnimInstance)->SetIsDie(true);

}

void APlayerCharacter::ShowGameOverUI()
{
	if (LevelUpUIWidget->IsInViewport())
	{
		LevelUpUIWidget->RemoveFromParent();
	}
	if (PlayerUIWidget->IsInViewport())
	{
		PlayerUIWidget->RemoveFromParent();
	}

	if (IsValid(GameOverUIWidgetClass))
	{
		GameOverUIWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverUIWidgetClass);
		if (IsValid(GameOverUIWidget))
		{
			GameOverUIWidget->AddToViewport();
		}
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(true);
	UWidgetBlueprintLibrary::SetInputMode_UIOnly(UGameplayStatics::GetPlayerController(GetWorld(), 0), GameOverUIWidget);

	SaveGame();
}

void APlayerCharacter::SaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist("MySaveGame", 0))
	{
		URoguelikeSaveGame* SaveGameInstance = Cast<URoguelikeSaveGame>(UGameplayStatics::CreateSaveGameObject(URoguelikeSaveGame::StaticClass()));

		if (SaveGameInstance)
		{
			SaveGameInstance = Cast<URoguelikeSaveGame>(UGameplayStatics::LoadGameFromSlot("MySaveGame", 0));
			SaveGameInstance->PlayerGold += Gold;
		}

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, "MySaveGame", 0);
	}
}

#pragma endregion



void APlayerCharacter::OnCriticalByMeleeAttack(AControlledCharacter* OtherActor)
{
	AnimInstance->Montage_Pause();

	AEliteMonster* IsElite = Cast<AEliteMonster>(OtherActor);

	if (IsElite != nullptr)
	{
		CriticalPauseTimer = 0.1f;
	}
	else
	{
		CriticalPauseTimer = 0.05f;
	}
}

void APlayerCharacter::KnockBack(float Amount, FVector From, FVector Additional)
{
	FRotator rotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation() * FVector(1, 1, 0), From * FVector(1, 1, 0));
	SetActorRotation(rotator);
	OnKnockBack();
	AnimInstance->Montage_Play(KnockBackAnim);
}

bool APlayerCharacter::GetIsInBossRoom()
{
	return IsInBossRoom;
}

void APlayerCharacter::SetIsInBossRoom(bool b)
{
	IsInBossRoom = b;
}

uint8 APlayerCharacter::GetPlayerLevel()
{
	return PlayerLevel;
}

UCapsuleComponent* APlayerCharacter::GetHitBox()
{
	return HitBox;
}


void APlayerCharacter::SetCanAttack(bool b)
{
	CanAttack = b;
}

float APlayerCharacter::GetStatusResultValue(EStatus type)
{
	if (type == EStatus::AD)
		return AControlledCharacter::GetStatusResultValue(type) * IncreaseADByWeaponType[(int)GetEquippingWeapon()->GetType()];
	return AControlledCharacter::GetStatusResultValue(type);
}