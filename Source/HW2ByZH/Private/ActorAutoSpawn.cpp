// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorAutoSpawn.h"

#include "EquipActor.h"

// Sets default values
AActorAutoSpawn::AActorAutoSpawn()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
}

// Called when the game starts or when spawned
void AActorAutoSpawn::BeginPlay()
{
	Super::BeginPlay();
	RespawnActor();
	
}

// Called every frame
void AActorAutoSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AActorAutoSpawn::RespawnActor()
{
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

