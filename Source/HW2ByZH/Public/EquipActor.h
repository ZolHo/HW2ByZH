// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquipActor.generated.h"

// 这两个代理多此一举了，不过学习了如何使用
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnterPickUpScopeDelegate,class AEquipActor*, EquipActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuitPickUpScopeDelegate,class AEquipActor*, EquipActor);
// DECLARE_DELEGATE(FOnActorDestoryDelegate);

UCLASS()
class HW2BYZH_API AEquipActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEquipActor();

	// 提示文本组件
	class UTextRenderComponent* HintTextRenderComponent;

	//获取Mesh
	virtual UMeshComponent* GetWeaponMesh();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// virtual void BeginDestroy() override;

	// Weapon Mesh
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Mesh)
	class UMeshComponent* WeaponMesh;

	// Box Collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Mesh)
	class UBoxComponent* BoxCollision;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FOnEnterPickUpScopeDelegate OnEnterScope;

	UPROPERTY(BlueprintAssignable)
	FOnQuitPickUpScopeDelegate OnQuitScope;
	
	// FOnActorDestoryDelegate OnActorDestory;

	// overlap 开始
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// overlap 结束
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
