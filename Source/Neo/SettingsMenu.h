// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NeoGameInstance.h"

#include "SettingsMenu.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API USettingsMenu : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

    UNeoGameInstance* GameInstance;

    UFUNCTION(BlueprintCallable)
    void Back();
};
