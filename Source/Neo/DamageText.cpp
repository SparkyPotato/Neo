// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageText.h"

// Sets default values
ADamageText::ADamageText()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TextComponent = CreateDefaultSubobject<UTextRenderComponent>("Text");
	RootComponent = TextComponent;
}

// Called when the game starts or when spawned
void ADamageText::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADamageText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADamageText::ActivateText(FText Text, AActor* ActorToFollow)
{
	TextComponent->SetText(Text);
	GetWorld()->GetTimerManager().SetTimer(DestroyTimer, this, &ADamageText::DestroyText, Time, false);
	Actor = ActorToFollow;
}

void ADamageText::DestroyText()
{
	this->Destroy();
}