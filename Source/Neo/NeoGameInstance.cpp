// Fill out your copyright notice in the Description page of Project Settings.

#include "NeoGameInstance.h"

void UNeoGameInstance::ShowMainMenu()
{
	if (MainMenuWidget == nullptr)
	{
		MainMenuWidget = UUserWidget::CreateWidgetInstance(*GetPrimaryPlayerController(), MainMenuWidgetClass, FName("MainMenu"));
	}
	MainMenuWidget->AddToViewport();
}

void UNeoGameInstance::ShowHostMenu()
{
	if (HostMenuWidget == nullptr)
	{
		HostMenuWidget = UUserWidget::CreateWidgetInstance(*GetPrimaryPlayerController(), HostMenuWidgetClass, FName("HostMenu"));
	}
	HostMenuWidget->AddToViewport();
}

void UNeoGameInstance::ShowServerMenu()
{
	ServerMenuWidget = UUserWidget::CreateWidgetInstance(*GetPrimaryPlayerController(), ServerMenuWidgetClass, FName("ServerMenu"));
	ServerMenuWidget->AddToViewport();
}

void UNeoGameInstance::ShowSettingsMenu()
{
	if (SettingsMenuWidget == nullptr)
	{
		SettingsMenuWidget = UUserWidget::CreateWidgetInstance(*GetPrimaryPlayerController(), SettingsMenuWidgetClass, FName("SettingsMenu"));
	}
	SettingsMenuWidget->AddToViewport();
}