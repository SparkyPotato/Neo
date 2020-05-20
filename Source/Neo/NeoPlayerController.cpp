// Fill out your copyright notice in the Description page of Project Settings.


#include "NeoPlayerController.h"

void ANeoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameAndUI());
}