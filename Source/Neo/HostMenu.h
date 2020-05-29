// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "HostMenu.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API UHostMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadWrite)
	FText ServerName;
	UPROPERTY(BlueprintReadWrite)
	bool bIsLAN;
	UPROPERTY(BlueprintReadWrite)
	int PlayerCount;

	UFUNCTION(BlueprintCallable)
	bool StartSession(FText Name, bool bLAN, int MaxPlayers);
};
