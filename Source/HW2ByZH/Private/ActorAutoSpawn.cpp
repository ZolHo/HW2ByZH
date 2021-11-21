// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorAutoSpawn.h"

#include "EquipActor.h"
#include "GunActor.h"

// Sets default values
AActorAutoSpawn::AActorAutoSpawn()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// ChildComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildComp"));
	// SetRootComponent(ChildComp);
	// 
	// RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	// SetRootComponent(RootSceneComponent);
}

// Called when the game starts or when spawned
void AActorAutoSpawn::BeginPlay()
{
	Super::BeginPlay();
	// if (ChildComp && ChildComp->GetChildActor())
	// {
	// 	ChildComp->GetChildActor()->OnDestroyed.AddDynamic(this, &AActorAutoSpawn::OnSpawnDestroyed);
	// }
	RespawnActor();
	
}

// Called every frame
void AActorAutoSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AActorAutoSpawn::RespawnActor()
{
	// if (ChildComp)
	// {
	// 	ChildComp->CreateChildActor();
	// 	if (ChildComp->GetChildActor())
	// 		ChildComp->GetChildActor()->OnDestroyed.AddDynamic(this, &AActorAutoSpawn::OnSpawnDestroyed);
	// 	if (!ChildComp->GetChildActor())
	// 	{
	// 		UE_LOG(LogTemp, Error, TEXT("Create Actor Faild"));
	// 	}
	// }
	
	UWorld* MyWorld = GetWorld();
	if (MyWorld)
	{
		EquipActor = MyWorld->SpawnActor<AEquipActor>(SpawnEquipType, this->GetActorLocation(), this->GetActorRotation());
	}
	
	if (!EquipActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Pleas Set The ActorClass of EquipAutoSpawn"))
	}
	else
	{
		EquipActor->OnDestroyed.AddDynamic(this, &AActorAutoSpawn::OnSpawnDestroyed);
	}
}

void AActorAutoSpawn::OnSpawnDestroyed(AActor* DestroyedActor)
{
	RespawnActor();

}

