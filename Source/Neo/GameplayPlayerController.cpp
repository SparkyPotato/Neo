// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayPlayerController.h"

void AGameplayPlayerController::SetSensitivity(float Sensitivity)
{
	PlayerInput->SetMouseSensitivity(Sensitivity / 25);
}
