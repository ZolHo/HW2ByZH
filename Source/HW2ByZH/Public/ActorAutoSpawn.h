// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorAutoSpawn.generated.h"

UCLASS()
class HW2BYZH_API AActorAutoSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorAutoSpawn();

	// 设置生产的装备
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// UClass* SpawnEquipType;
	TSubclassOf<AActor> SpawnEquipType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* ChildComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Component)
	USceneComponent* RootSceneComponent;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

	// 重新生产Actor
	UFUNCTION()
	void RespawnActor();

	UPROPERTY()
	class AEquipActor* EquipActor;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnSpawnDestroyed(AActor* DestroyedActor);

};