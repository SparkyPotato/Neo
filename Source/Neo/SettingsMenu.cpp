// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenu.h"

void USettingsMenu::NativeOnInitialized()
{
	Super::OnInitialized();

	GameInstance = GetGameInstance<UNeoGameInstance>();
}

void USettingsMenu::Back()
{
	GameInstance->ShowMainMenu();
	RemoveFromParent();
}