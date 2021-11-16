// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipActor.h"
#include "GameFramework/Actor.h"
#include "LeiActor.generated.h"

UCLASS()
class HW2BYZH_API ALeiActor : public AEquipActor
{
	GENERATED_BODY()

public:
	ALeiActor();
	
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
};
