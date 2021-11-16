// Fill out your copyright notice in the Description page of Project Settings.


#include "LeiActor.h"

#include "GunActor.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"


void ALeiActor::BeginPlay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	Super::BeginPlay();
}

// 设置默认构造函数
ALeiActor::ALeiActor()
{
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	
	// 设置骨骼
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssert(TEXT("StaticMesh'/Game/ThirdPersonCPP/ShouLei/shoulei.shoulei'"));
	if (MeshAssert.Succeeded())
	{
		Cast<UStaticMeshComponent>(WeaponMesh)->SetStaticMesh(MeshAssert.Object);
	}
	
	BoxCollision->SetupAttachment(WeaponMesh);
	HintTextRenderComponent->SetupAttachment(WeaponMesh);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALeiActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
