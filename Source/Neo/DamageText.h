// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"

#include "DamageText.generated.h"

UCLASS()
class NEO_API ADamageText : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageText();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle DestroyTimer;
	void DestroyText();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	UTextRenderComponent* TextComponent;
	UPROPERTY(EditDefaultsOnly)
	float Time;

	UPROPERTY(BlueprintReadOnly)
	AActor* Actor;

	void ActivateText(FText Text, AActor* ActorToFollow);
};
