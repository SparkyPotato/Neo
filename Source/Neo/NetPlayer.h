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
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float DashCooldown;
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float DashStrength;
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float PushoffCooldown;
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float PushoffStrength;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float UnCrouchCooldown;

	// Components
	UPROPERTY(EditDefaultsOnly)
	UCameraComponent* CameraComponent;
	UCapsuleComponent* CollsionComponent;
	UPROPERTY(EditDefaultsOnly)
	UCapsuleComponent* DetectionComponent;
	USkeletalMeshComponent* MeshComponent;
	UCharacterMovementComponent* MovementComponent;

	// Control variables
	int JumpsLeft;

	bool bCanDash;
	FTimerHandle DashTimer;

	bool bCanPushoff;
	FTimerHandle PushoffTimer;

	bool bCanUnCrouch;
	bool bTriedToUnCrouch;
	FTimerHandle UnCrouchTimer;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Movement functions
	void Forward(float Speed);
	void Right(float Speed);

	void MoveMouseX(float Speed);
	void MoveMouseY(float Speed);

	void ClientJump();

	void ClientCrouch();
	UFUNCTION(Server, Reliable)
	void ServerCrouch();
	void ClientUnCrouch();
	UFUNCTION(Server, Reliable)
	void ServerUnCrouch();
	void AllowUnCrouch();
	UFUNCTION(NetMulticast, Reliable)
	void SetScale(FVector Scale);

	void ClientDash();
	void EndDash();

	void ClientPushoff();
	void EndPushoff();

	// Events
	UFUNCTION()
	void OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};