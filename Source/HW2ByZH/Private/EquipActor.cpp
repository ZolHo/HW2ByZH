// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipActor.h"

#include "GunActor.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "HW2ByZH/HW2ByZHCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEquipActor::AEquipActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 设置提示文本
	HintTextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HintText"));
	HintTextRenderComponent->SetWorldSize(20.f);
	HintTextRenderComponent->SetTextRenderColor(FColor::White);
	HintTextRenderComponent->SetText(INVTEXT("Press F To Pick UP"));
	HintTextRenderComponent->SetRelativeLocation(FVector(50, 0, 0));
	HintTextRenderComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	
	// BoxCollision
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollison"));
	
	// 设置重叠事件
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AGunActor::OnOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AGunActor::OnOverlapEnd);
	BoxCollision->SetCollisionProfileName(FName("OverlapOnlyPawn"));

	// 默认hide提示文本
	HintTextRenderComponent->SetVisibility(false);
}

// Called when the game starts or when spawned
void AEquipActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEquipActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UMeshComponent* AEquipActor::GetWeaponMesh()
{
	return WeaponMesh;
}

void AEquipActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "Overlay");
	// 维护附件武器集合
	Cast<AHW2ByZHCharacter>(UGameplayStatics::GetPlayerPawn(this, 0))->WeaponWhichCanPickSet.Add(this);
	HintTextRenderComponent->SetVisibility(true);

}

void AEquipActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, "OverlapEnd");
	//维护附件武器集合
	Cast<AHW2ByZHCharacter>(UGameplayStatics::GetPlayerPawn(this, 0))->WeaponWhichCanPickSet.Remove(this);
	HintTextRenderComponent->SetVisibility(false);
}


