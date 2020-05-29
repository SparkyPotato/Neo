// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuPlayerController.h"

AMenuPlayerController::AMenuPlayerController()
{
	
}

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = GetGameInstance<UNeoGameInstance>();

	GameInstance->ShowMainMenu();
}