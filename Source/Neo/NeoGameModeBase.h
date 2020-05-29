// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/UserWidget.h"

#include "NeoGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API ANeoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

protected:
	/** Called when the game starts. */
	virtual void BeginPlay() override;
};
