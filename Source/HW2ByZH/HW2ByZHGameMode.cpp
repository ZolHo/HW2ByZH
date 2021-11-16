// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW2ByZHGameMode.h"
#include "HW2ByZHCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHW2ByZHGameMode::AHW2ByZHGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
