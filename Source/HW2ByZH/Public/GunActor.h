// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipActor.h"
#include "GunActor.generated.h"



UCLASS()
class HW2BYZH_API AGunActor : public AEquipActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunActor();

	// 枪的弹匣容量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Amor)
	int GunMagazineLimit = 15;

	// 当前子弹数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Amor)
	int GunAmorNumber = 15;

	// 弹舱子弹数目
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Amor)
	int MagazineAmorNumber = 60;

	// 换弹函数
	UFUNCTION()
	void GunReBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
