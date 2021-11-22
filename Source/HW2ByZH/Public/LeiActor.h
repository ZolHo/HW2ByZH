// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipActor.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LeiActor.generated.h"

UCLASS()
class HW2BYZH_API ALeiActor : public AEquipActor
{
	GENERATED_BODY()

public:
	ALeiActor();

	// // 抛射组件 （失败，因为初始化决定了抛射参数不能更改）
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Move")
	// UProjectileMovementComponent* ProjectileMovementComponent;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move")
	// int ProjectileInitSpeed;

	// 倒计时引爆
	UFUNCTION()
	void ReadyToBoom(float DelayTime = 3.f);

	// 爆炸
	UFUNCTION(BlueprintImplementableEvent, Category="Boom")
	void LeiBoom();
	
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
};
