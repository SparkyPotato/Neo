// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NeoGameInstance.h"

#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

	UNeoGameInstance* GameInstance;

	UFUNCTION(BlueprintCallable)
	void ShowHost();
	UFUNCTION(BlueprintCallable)
	void ShowServers();
	UFUNCTION(BlueprintCallable)
	void ShowSettings();
};
