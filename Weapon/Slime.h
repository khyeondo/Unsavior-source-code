// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Slime.generated.h"

UCLASS()
class TEAMPROJECT_API ASlime : public APawn
{
	GENERATED_BODY()

protected:
	class APlayerCharacter* Player;

	uint8 WeaponSlotNum;
	
	UPROPERTY(BlueprintReadOnly)
	bool IsEmpty;

	UPROPERTY(BlueprintReadOnly)
	TArray<class AWeapon*> WeaponSlot;
	uint8 CurWeaponIdx;

public:
	// Sets default values for this pawn's properties
	ASlime();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	bool ChangeWeapon(uint8 idx);                            //���� �� ���� ���� 
	UFUNCTION(BlueprintCallable)
	bool AddWeapon(TSubclassOf<class AWeapon> weaponType);     //���� ȹ��
	UFUNCTION(BlueprintCallable)
	void AddAltarWeapon(TSubclassOf<class AWeapon> weaponType);
	void AddWeapon_Implementation(int index, TSubclassOf<class AWeapon> weaponType, bool b);
	UFUNCTION(Blueprintpure)
    AWeapon* GetEquippingWeapon();

	UFUNCTION(BlueprintCallable)
	void AttachToPlayer(APlayerCharacter* player);

	UFUNCTION(Blueprintpure)
	TArray<class AWeapon*> GetWeaponSlot();

	UFUNCTION(Blueprintpure)
	bool GetIsEmpty();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
