// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CanBeGunHitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCanBeGunHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HW2BYZH_API ICanBeGunHitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent)
	void WhenBeGunHit(const FHitResult& HitResult);
};
