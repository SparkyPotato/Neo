// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

void UMainMenu::NativeOnInitialized()
{
	Super::OnInitialized();

	GameInstance = GetGameInstance<UNeoGameInstance>();
}

void UMainMenu::ShowHost()
{
	RemoveFromParent();
	GameInstance->ShowHostMenu();
}

void UMainMenu::ShowServers()
{
	RemoveFromParent();
	GameInstance->ShowServerMenu();
}

void UMainMenu::ShowSettings()
{
	RemoveFromParent();
	GameInstance->ShowSettingsMenu();
}