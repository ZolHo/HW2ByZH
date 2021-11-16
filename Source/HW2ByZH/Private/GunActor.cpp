// Fill out your copyright notice in the Description page of Project Settings.


#include "GunActor.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"

// Sets default values
AGunActor::AGunActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	WeaponMesh = Cast<USkeletalMeshComponent, UMeshComponent>(WeaponMesh);
	
	// 设置骨骼
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAssert(TEXT("SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (MeshAssert.Succeeded())
	{
		Cast<USkeletalMeshComponent>(WeaponMesh)->SetSkeletalMesh(MeshAssert.Object);
	}

	BoxCollision->SetRelativeScale3D(FVector(3,3,3));
	BoxCollision->SetupAttachment(WeaponMesh);
	HintTextRenderComponent->SetupAttachment(WeaponMesh);
	
}

// Called when the game starts or when spawned
void AGunActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGunActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
