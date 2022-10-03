// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "CustomDataTable.generated.h"

USTRUCT(BlueprintType)
struct FMonsterData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FMonsterData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		FString Class;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Armor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float AD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Burn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Freezing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Electric;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		FString Element;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Exp;
};

UCLASS()
class TEAMPROJECT_API ACustomDataTable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACustomDataTable();

};
