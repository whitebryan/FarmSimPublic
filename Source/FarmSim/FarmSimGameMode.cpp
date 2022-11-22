// Copyright Epic Games, Inc. All Rights Reserved.

#include "FarmSimGameMode.h"
#include "FarmSimCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFarmSimGameMode::AFarmSimGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
