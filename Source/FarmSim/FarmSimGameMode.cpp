// Copyright Epic Games, Inc. All Rights Reserved.

#include "FarmSimGameMode.h"
#include "Player/FarmSimCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFarmSimGameMode::AFarmSimGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
