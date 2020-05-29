// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameUserSettings.h"

#include "NeoGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API UNeoGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	UGameUserSettings* GameSettings;

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UFUNCTION(BlueprintCallable)
	void ShowMainMenu();
	UUserWidget* MainMenuWidget = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HostMenuWidgetClass;
	UFUNCTION(BlueprintCallable)
	void ShowHostMenu();
	UUserWidget* HostMenuWidget = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ServerMenuWidgetClass;
	UFUNCTION(BlueprintCallable)
	void ShowServerMenu();
	UUserWidget* ServerMenuWidget = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> SettingsMenuWidgetClass;
	UFUNCTION(BlueprintCallable)
	void ShowSettingsMenu();
	UUserWidget* SettingsMenuWidget = nullptr;
};
