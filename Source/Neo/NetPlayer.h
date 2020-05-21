// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/RotationMatrix.h"
#include "Math/Vector.h"
#include "Net/UnrealNetwork.h"

#include "NetPlayer.generated.h"

UCLASS()
class NEO_API ANetPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	ANetPlayer();

protected:
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Settings
	UPROPERTY(EditAnywhere, Category="Settings")
	float Sensitivity;

	// Variables
	UPROPERTY(EditAnywhere, Category="Movement")
	float DashCooldown;
	UPROPERTY(EditAnywhere, Category="Movement")
	float PushoffCooldown;

	// Components
	UPROPERTY(EditDefaultsOnly)
	UCameraComponent* CameraComponent;
	UCapsuleComponent* CollsionComponent;
	UPROPERTY(EditDefaultsOnly)
	UCapsuleComponent* DetectionComponent;
	USkeletalMeshComponent* MeshComponent;
	UCharacterMovementComponent* MovementComponent;

	// Control variables
	UPROPERTY(Replicated)
	int JumpsLeft;

	bool bCanDash;
	FTimerHandle DashTimer;

	bool bCanPushoff;
	FTimerHandle PushoffTimer;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Movement functions
	void Forward(float Speed);
	void Right(float Speed);

	void MoveMouseX(float Speed);
	void MoveMouseY(float Speed);

	void ClientJump();
	UFUNCTION(Server, Reliable)
	void ServerJump();

	void ClientCrouch();
	UFUNCTION(Server, Reliable)
	void ServerCrouch();
	void ClientUncrouch();
	UFUNCTION(Server, Reliable)
	void ServerUncrouch();
	UFUNCTION(NetMulticast, Reliable)
	void SetScale(FVector Scale);

	void ClientDash();
	void EndDash();
	UFUNCTION(Server, Reliable)
	void ServerDash();

	void ClientPushoff();
	void EndPushoff();
	UFUNCTION(Server, Reliable)
	void ServerPushoff();

	// Events
	UFUNCTION()
	void OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};