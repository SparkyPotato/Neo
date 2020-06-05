// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerInput.h"

#include "GameplayPlayerController.generated.h"

/**
 *
 */
UCLASS()
class NEO_API AGameplayPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetSensitivity(float Sensitivity);
};
