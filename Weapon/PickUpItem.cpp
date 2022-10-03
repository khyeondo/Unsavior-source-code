// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItem.h"
#include "Components/StaticMeshComponent.h"
#include "HitBoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

// Sets default values
APickUpItem::APickUpItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = StaticMesh;

	HitBox = CreateDefaultSubobject<UHitBoxComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(StaticMesh);
	//HitBox->SetIsCheckBeginOverlapOnce(true);
}

// Called when the game starts or when spawned
void APickUpItem::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerController(this, 0)->GetPawn());

}

void APickUpItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HitBox->SetTargetToPlayer();
	HitBox->EnableCollision();

	HitBox->Func_HitColliderBeginOverlap.AddDynamic(this, &APickUpItem::OnHitCollisionBeginOverlap);
}

// Called every frame
void APickUpItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickUpItem::OnHitCollisionBeginOverlap(AControlledCharacter* OtherActor, const FHitResult& SweepResult)
{
	OnPickUp();
}

void APickUpItem::OnPickUp_Implementation()
{
	Destroy();
}