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
	PrimaryActorTick.bCanEverTick = true;
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	RootComponent = WeaponMesh;
	// 设置骨骼，不考虑蓝图子类的话使用代码绑定模型
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssert(TEXT("StaticMesh'/Game/ThirdPersonCPP/ShouLei/shoulei.shoulei'"));
	if (MeshAssert.Succeeded())
	{
		Cast<UStaticMeshComponent>(WeaponMesh)->SetStaticMesh(MeshAssert.Object);
	}

	// 自定义组件属性如碰撞体积，提示文本
	BoxCollision->SetRelativeScale3D(FVector(2,2,2));
	BoxCollision->SetupAttachment(WeaponMesh);
	HintTextRenderComponent->SetupAttachment(WeaponMesh);

	WeaponMesh->SetCollisionProfileName("OverlapAll");
	
	// 配置抛射组件
	// ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("projectleComponent"));
	// if (ProjectileInitSpeed)
	// 	ProjectileMovementComponent->InitialSpeed = ProjectileInitSpeed;
	// else
	// 	ProjectileMovementComponent->InitialSpeed = 1000.f;
	// ProjectileMovementComponent->bRotationFollowsVelocity = false;
	// ProjectileMovementComponent->bShouldBounce = true;
	// ProjectileMovementComponent->bSimulationEnabled = false;
	//
}

void ALeiActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALeiActor::ReadyToBoom(float DelayTime)
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ALeiActor::LeiBoom, DelayTime, false, -1.0f);
}

// void ALeiActor::LeiBoom()
// {
// 	UE_LOG(LogTemp, Warning, TEXT("Boom!!!!"));
// }
