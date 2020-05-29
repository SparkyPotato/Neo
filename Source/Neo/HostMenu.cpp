// Fill out your copyright notice in the Description page of Project Settings.


#include "HostMenu.h"

void UHostMenu::NativeOnInitialized()
{
	PlayerCount = 2;
}

bool UHostMenu::StartSession(FText Name, bool bLAN, int MaxPlayers)
{
	FOnlineSessionSettings Settings;
	Settings.bIsLANMatch = bLAN;
	Settings.NumPublicConnections = MaxPlayers;

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem)
	{
		return false;
	}
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (!Session)
	{
		return false;
	}
	Session->CreateSession(0, FName(Name.ToString()), Settings);

	//UGameplayStatics::OpenLevel(GetWorld()->GetFirstPlayerController(), FName("Lobby"), false, FString("?listen"));

	return true;
}
