// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/RotationMatrix.h"
#include "Math/Vector.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "DrawDebugHelpers.h"
#include "Sound/SoundWave.h"
#include "Bullet.h"
#include "DamageText.h"
#include "GameplayPlayerController.h"

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
	UPROPERTY(EditAnywhere, Category = "Settings")
	float Sensitivity;

	// Variables
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	int MaxHealth;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	TSubclassOf<ADamageText> DamageText;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	USoundWave* HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	USoundWave* DashSound;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float DashCooldown;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float DashStrength;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	USoundWave* PushoffSound;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float PushoffCooldown;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float PushoffStrength;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	USoundWave* JumpSound;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	USoundWave* LandSound;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float UnCrouchCooldown;

	UPROPERTY(EditDefaultsOnly, Category = "Pistol")
	int PistolDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Pistol")
	float PistolFirerate;
	UPROPERTY(EditDefaultsOnly, Category = "Pistol")
	int PistolMaxAmmo;
	UPROPERTY(EditDefaultsOnly, Category = "Pistol")
	float PistolReloadTime;
	UPROPERTY(EditDefaultsOnly, Category = "Pistol")
	UStaticMesh* PistolMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Pistol")
	FVector PistolOffset;
	UPROPERTY(EditDefaultsOnly, Category = "Pistol")
	TSubclassOf<ABullet> PistolBullet;
	UPROPERTY(EditDefaultsOnly, Category = "Pistol")
	USoundWave* PistolAudio;

	// Components
	UPROPERTY(EditDefaultsOnly)
	UCameraComponent* CameraComponent;
	UCapsuleComponent* CollisionComponent;
	UPROPERTY(EditDefaultsOnly)
	UCapsuleComponent* DetectionComponent;
	USkeletalMeshComponent* MeshComponent;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* WeaponMeshComponent;
	UCharacterMovementComponent* MovementComponent;
	UPROPERTY(EditDefaultsOnly)
	UAudioComponent* GunAudioComponent;
	UPROPERTY(EditDefaultsOnly)
	UAudioComponent* HitAudioComponent;
	UPROPERTY(EditDefaultsOnly)
	UAudioComponent* HurtAudioComponent;
	UPROPERTY(EditDefaultsOnly)
	UAudioComponent* MovementAudioComponent;

	// Control variables
	int JumpsLeft;

	bool bCanDash;
	FTimerHandle DashTimer;

	bool bCanPushoff;
	FTimerHandle PushoffTimer;

	bool bCanUnCrouch;
	bool bTriedToUnCrouch;
	FTimerHandle UnCrouchTimer;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth)
	int CurrentHealth;
	UFUNCTION()
	void OnRep_CurrentHealth();

	bool bCanFirePistol;
	bool bIsReloadingPistol;
	FTimerHandle PistolTimer;
	float PistolReloadTimer;
	int PistolAmmo;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void UpdateHealth();

	UPROPERTY(BlueprintReadWrite)
	bool bIsDead;
	FTimerHandle RespawnTimer;
	UFUNCTION(Server, Reliable)
	void KillPlayer();
	void ClientRespawn();
	UFUNCTION(Server, Reliable)
	void Respawn();

	UPROPERTY(BlueprintReadOnly, Replicated)
	int Kills;
	UPROPERTY(BlueprintReadOnly, Replicated)
	int Deaths;

	UFUNCTION(Server, Reliable)
	void IncrementKills();
	UFUNCTION(Server, Reliable)
	void IncrementDeaths();

	//Getter functions
	UFUNCTION(BlueprintCallable)
	int GetCurrentHealth() { return CurrentHealth; }
	UFUNCTION(BlueprintCallable)
	int GetPistolAmmo() { return PistolAmmo; }
	UFUNCTION(BlueprintCallable)
	int GetPistolMaxAmmo() { return PistolMaxAmmo; }
	UFUNCTION(BlueprintCallable)
	bool GetIsReloadingPistol() { return bIsReloadingPistol; }

	// Movement functions
	void Forward(float Speed);
	void Right(float Speed);

	void MoveMouseX(float Speed);
	void MoveMouseY(float Speed);

	void ClientJump();
	UFUNCTION(Reliable, Server)
	void ServerJump();

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
	UFUNCTION(Reliable, Server)
	void ServerDash();
	void EndDash();

	void ClientPushoff();
	UFUNCTION(Reliable, Server)
	void ServerPushoff();
	void EndPushoff();

	// Weapon functions
	void Fire();
	void Reload();
	UFUNCTION(Server, Reliable)
	void Damage(ANetPlayer* PlayerToDamage, int Damage);
	UFUNCTION(Server, Unreliable)
	void SpawnPistolBullet(FVector Location, FRotator Rotation, FVector Target);
	UFUNCTION(Server, Unreliable)
	void SetWeaponRotation(FRotator Rotation);

	void PistolFire();
	void PistolEndFire();
	void PistolReload();

	// Sound functions
	UFUNCTION(Server, Reliable)
	void PlayOnServer(UAudioComponent* ComponentToPlay);
	UFUNCTION(NetMulticast, Reliable)
	void PlayOnClients(UAudioComponent* ComponentToPlay);

	// Events
	UFUNCTION()
	void OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};