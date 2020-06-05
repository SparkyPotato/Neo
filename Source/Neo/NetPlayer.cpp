// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayer.h"

ANetPlayer::ANetPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	DetectionComponent = CreateDefaultSubobject<UCapsuleComponent>("Detection");
	DetectionComponent->SetCapsuleSize(44.f, 106.f);
	DetectionComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight));
	CameraComponent->SetupAttachment(GetRootComponent());

	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	WeaponMeshComponent->SetupAttachment(CameraComponent);

	GunAudioComponent = CreateDefaultSubobject<UAudioComponent>("GunAudio");
	GunAudioComponent->SetupAttachment(WeaponMeshComponent);

	HitAudioComponent = CreateDefaultSubobject<UAudioComponent>("HitAudio");
	HitAudioComponent->SetupAttachment(CameraComponent);

	HurtAudioComponent = CreateDefaultSubobject<UAudioComponent>("HurtAudio");
	HurtAudioComponent->SetupAttachment(CameraComponent);

	MovementAudioComponent = CreateDefaultSubobject<UAudioComponent>("MovementAudio");
	MovementAudioComponent->SetupAttachment(GetMesh());

	bIsDead = false;
}

void ANetPlayer::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent = FindComponentByClass<UCapsuleComponent>();
	MeshComponent = FindComponentByClass<USkeletalMeshComponent>();
	MovementComponent = FindComponentByClass<UCharacterMovementComponent>();

	DetectionComponent->OnComponentBeginOverlap.AddDynamic(this, &ANetPlayer::OnDetectionOverlapBegin);
	DetectionComponent->OnComponentEndOverlap.AddDynamic(this, &ANetPlayer::OnDetectionOverlapEnd);

	WeaponMeshComponent->SetStaticMesh(PistolMesh);
	WeaponMeshComponent->SetRelativeLocation(PistolOffset);

	JumpsLeft = JumpMaxCount;
	bCanPushoff = true;
	bCanDash = true;
	bCanUnCrouch = false;
	bTriedToUnCrouch = false;

	CurrentHealth = MaxHealth;
	HitAudioComponent->SetSound(HitSound);

	bCanFirePistol = true;
	bIsReloadingPistol = false;
	PistolAmmo = PistolMaxAmmo;
	GunAudioComponent->SetSound(PistolAudio);
}

void ANetPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetPlayer, CurrentHealth);
	DOREPLIFETIME(ANetPlayer, Kills);
	DOREPLIFETIME(ANetPlayer, Deaths);
}

void ANetPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsReloadingPistol)
	{
		if (PistolReloadTimer >= PistolReloadTime - PistolReloadTime / 4)
		{
			WeaponMeshComponent->SetRelativeRotation(FMath::RInterpConstantTo(WeaponMeshComponent->GetRelativeRotation(), FRotator(0.f, 0.f, 0.f), DeltaTime, 360.f / PistolReloadTime));
			SetWeaponRotation(FMath::RInterpConstantTo(WeaponMeshComponent->GetRelativeRotation(), FRotator(0.f, 0.f, 0.f), DeltaTime, 360.f / PistolReloadTime));
			if (FMath::RoundHalfFromZero(WeaponMeshComponent->GetRelativeRotation().Pitch) == 0.f)
			{
				bIsReloadingPistol = false;
				PistolReloadTimer = 0.f;
				PistolAmmo = PistolMaxAmmo;
			}
		}
		else
		{
			PistolReloadTimer += DeltaTime;
			WeaponMeshComponent->SetRelativeRotation(FMath::RInterpConstantTo(WeaponMeshComponent->GetRelativeRotation(), FRotator(90.f, 0.f, 0.f), DeltaTime, 360.f / PistolReloadTime));
			SetWeaponRotation(FMath::RInterpConstantTo(WeaponMeshComponent->GetRelativeRotation(), FRotator(90.f, 0.f, 0.f), DeltaTime, 360.f / PistolReloadTime));
		}
	}
}

void ANetPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Forward", this, &ANetPlayer::Forward);
	PlayerInputComponent->BindAxis("Right", this, &ANetPlayer::Right);

	PlayerInputComponent->BindAxis("MouseX", this, &ANetPlayer::MoveMouseX);
	PlayerInputComponent->BindAxis("MouseY", this, &ANetPlayer::MoveMouseY);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ANetPlayer::ClientJump);

	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ANetPlayer::ClientCrouch);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &ANetPlayer::ClientUnCrouch);

	PlayerInputComponent->BindAction("Dash", EInputEvent::IE_Pressed, this, &ANetPlayer::ClientDash);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_DoubleClick, this, &ANetPlayer::ClientPushoff);

	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ANetPlayer::Fire);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &ANetPlayer::Reload);
}

void ANetPlayer::Forward(float Speed)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Speed * MovementComponent->GetMaxSpeed());
}

void ANetPlayer::Right(float Speed)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Speed * MovementComponent->GetMaxSpeed());
}

void ANetPlayer::MoveMouseX(float Speed)
{
	AddControllerYawInput(Speed * Sensitivity);
}

void ANetPlayer::MoveMouseY(float Speed)
{
	AddControllerPitchInput(Speed * Sensitivity);
}

void ANetPlayer::ClientJump()
{
	if (JumpsLeft > 0)
	{
		ServerJump();
		LaunchCharacter(FVector(0.f, 0.f, MovementComponent->JumpZVelocity), false, true);
		MovementAudioComponent->SetSound(JumpSound);
		MovementAudioComponent->Play(0.f);
		JumpsLeft--;
	}
}

void ANetPlayer::ServerJump_Implementation()
{
	LaunchCharacter(FVector(0.f, 0.f, MovementComponent->JumpZVelocity), false, true);
	MovementAudioComponent->SetSound(JumpSound);
	MovementAudioComponent->Play(0.f);
}

void ANetPlayer::ClientCrouch()
{
	bCanUnCrouch = false;
	GetWorld()->GetTimerManager().SetTimer(UnCrouchTimer, this, &ANetPlayer::AllowUnCrouch, UnCrouchCooldown, false);
	Crouch();
	SetActorScale3D(FVector(1.f, 1.f, 0.5f));
	WeaponMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, 2.f));
	ServerCrouch();
	MovementComponent->SetWalkableFloorAngle(5.f);
	MovementComponent->BrakingFriction = 0.1f;
	MovementComponent->BrakingDecelerationWalking = 128.f;
	MovementComponent->GroundFriction = 0.f;
}

void ANetPlayer::ServerCrouch_Implementation()
{
	Crouch();
	SetScale(FVector(1.f, 1.f, 0.5f));
	WeaponMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, 2.f));
	MovementComponent->SetWalkableFloorAngle(5.f);
	MovementComponent->BrakingFriction = 0.1f;
	MovementComponent->BrakingDecelerationWalking = 128.f;
	MovementComponent->GroundFriction = 0.f;
}

void ANetPlayer::ClientUnCrouch()
{
	if (!bCanUnCrouch)
	{
		bTriedToUnCrouch = true;
		return;
	}
	UnCrouch();
	SetActorScale3D(FVector(1.f, 1.f, 1.f));
	WeaponMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	ServerUnCrouch();
	MovementComponent->SetWalkableFloorAngle(60.f);
	MovementComponent->BrakingFriction = 1.f;
	MovementComponent->BrakingDecelerationWalking = 2048.f;
	MovementComponent->GroundFriction = 4.f;
}

void ANetPlayer::AllowUnCrouch()
{
	bCanUnCrouch = true;
	if (bTriedToUnCrouch)
	{
		ClientUnCrouch();
		bTriedToUnCrouch = false;
	}
}

void ANetPlayer::ServerUnCrouch_Implementation()
{
	UnCrouch();
	SetScale(FVector(1.f, 1.f, 1.f));
	WeaponMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	MovementComponent->SetWalkableFloorAngle(60.f);
	MovementComponent->BrakingFriction = 1.f;
	MovementComponent->BrakingDecelerationWalking = 2048.f;
	MovementComponent->GroundFriction = 4.f;
}

void ANetPlayer::SetScale_Implementation(FVector Scale)
{
	CollisionComponent->SetRelativeScale3D(Scale);
}

void ANetPlayer::ClientDash()
{
	if (bCanDash)
	{
		bCanDash = false;
		GetWorld()->GetTimerManager().SetTimer(DashTimer, this, &ANetPlayer::EndDash, DashCooldown, false);
		MovementAudioComponent->SetSound(DashSound);
		MovementAudioComponent->Play(0.f);
		ServerDash();
		LaunchCharacter((FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X)) * MovementComponent->JumpZVelocity * DashStrength, true, true);
	}
}

void ANetPlayer::ServerDash_Implementation()
{
	LaunchCharacter((FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X)) * MovementComponent->JumpZVelocity * DashStrength, true, true);
	MovementAudioComponent->SetSound(DashSound);
	MovementAudioComponent->Play(0.f);
}

void ANetPlayer::EndDash()
{
	bCanDash = true;
}

void ANetPlayer::ClientPushoff()
{
	if (bCanPushoff)
	{
		bCanPushoff = false;
		GetWorld()->GetTimerManager().SetTimer(PushoffTimer, this, &ANetPlayer::EndPushoff, PushoffCooldown, false);
		MovementAudioComponent->SetSound(PushoffSound);
		MovementAudioComponent->Play(0.f);
		ServerPushoff();
		LaunchCharacter(FVector(0.f, 0.f, MovementComponent->JumpZVelocity * PushoffStrength), true, true);
	}
}

void ANetPlayer::ServerPushoff_Implementation()
{
	LaunchCharacter(FVector(0.f, 0.f, MovementComponent->JumpZVelocity * PushoffStrength), true, true);
	MovementAudioComponent->SetSound(PushoffSound);
	MovementAudioComponent->Play(0.f);
}

void ANetPlayer::EndPushoff()
{
	bCanPushoff = true;
}

void ANetPlayer::OnRep_CurrentHealth()
{
	UpdateHealth();
	if (CurrentHealth < 100)
	{
		HurtAudioComponent->Play(0.f);
	}
}

void ANetPlayer::UpdateHealth()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Health: %i"), *GetName(), CurrentHealth);
	if (CurrentHealth <= 0)
	{
		KillPlayer();
	}
}

void ANetPlayer::IncrementKills_Implementation()
{
	Kills++;
}

void ANetPlayer::IncrementDeaths_Implementation()
{
	Deaths++;
}

void ANetPlayer::KillPlayer_Implementation()
{
	IncrementDeaths();
	bIsDead = true;
	SetActorLocation(FVector(100000.f, 100000.f, 100000.f));
	GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &ANetPlayer::ClientRespawn, 5.f, false);
}

void ANetPlayer::ClientRespawn()
{
	bCanPushoff = true;
	bCanDash = true;
	bCanUnCrouch = false;
	bTriedToUnCrouch = false;
	bCanFirePistol = true;
	bIsReloadingPistol = false;
	PistolAmmo = PistolMaxAmmo;
	Respawn();
}

void ANetPlayer::Respawn_Implementation()
{
	CurrentHealth = MaxHealth;
	bIsDead = false;
	SetActorLocation(FVector(0.f, 0.f, 200.f));
}

void ANetPlayer::Damage_Implementation(ANetPlayer* PlayerToDamage, int Damage)
{
	PlayerToDamage->CurrentHealth -= Damage;
	PlayerToDamage->UpdateHealth();
}

void ANetPlayer::SpawnPistolBullet_Implementation(FVector Location, FRotator Rotation, FVector Target)
{
	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(PistolBullet, Location, Rotation, SpawnParams);
	Bullet->Activate(Target);
	GunAudioComponent->Play(0.f);
}

void ANetPlayer::SetWeaponRotation_Implementation(FRotator Rotation)
{
	WeaponMeshComponent->SetRelativeRotation(Rotation);
}

void ANetPlayer::Fire()
{
	PistolFire();
}

void ANetPlayer::Reload()
{
	if (PistolAmmo < PistolMaxAmmo)
	{
		PistolReload();
	}
}

void ANetPlayer::PistolFire()
{
	if (bCanFirePistol && !bIsReloadingPistol)
	{
		bCanFirePistol = false;
		PistolAmmo--;
		GetWorld()->GetTimerManager().SetTimer(PistolTimer, this, &ANetPlayer::PistolEndFire, PistolFirerate, false);

		FHitResult Hit;
		FVector Start;
		if (MovementComponent->IsCrouching())
		{
			Start = GetActorLocation() + CameraComponent->GetRelativeLocation() + CameraComponent->GetForwardVector() * 165.f - FVector(0.f, 0.f, 32.f);
		}
		else
		{
			Start = GetActorLocation() + CameraComponent->GetRelativeLocation() + CameraComponent->GetForwardVector() * 200.f;
		}
		FVector End = Start + CameraComponent->GetForwardVector() * 9000.f;
		GunAudioComponent->Play(0.f);
		FCollisionObjectQueryParams CollisionParams = FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_GameTraceChannel1));
		if (GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, CollisionParams))
		{
			FActorSpawnParameters SpawnParams = FActorSpawnParameters();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			FVector SpawnLocation = WeaponMeshComponent->GetComponentLocation() + CameraComponent->GetForwardVector() * 30.f;
			FRotator SpawnRotation = CameraComponent->GetRelativeRotation() + GetActorRotation();
			SpawnPistolBullet(SpawnLocation, SpawnRotation, Hit.Location);
			if (Hit.GetActor()->ActorHasTag(FName("Player")) && Hit.GetActor() != this && Hit.IsValidBlockingHit())
			{
				FVector RandVect = FMath::VRand() * 100.f;
				ADamageText* DamageTextInstance = GetWorld()->SpawnActor<ADamageText>(DamageText, Hit.GetActor()->GetActorLocation() + 50.f * -CameraComponent->GetForwardVector() + 
					FVector(0.f, 0.f, 100.f) + FVector::VectorPlaneProject(RandVect, CameraComponent->GetForwardVector()), CameraComponent->GetRelativeRotation() + GetActorRotation() + FRotator(0.f, 180.f, 0.f), SpawnParams);
				DamageTextInstance->ActivateText(FText::AsNumber(PistolDamage), this);
				HitAudioComponent->Play(0.f);
				Damage(Cast<ANetPlayer>(Hit.GetActor()), PistolDamage);
				if ((Cast<ANetPlayer>(Hit.GetActor()))->CurrentHealth <= 0)
				{
					IncrementKills();
				}
			}
		}
		if (PistolAmmo == 0)
		{
			PistolReload();
		}
	}
}

void ANetPlayer::PistolEndFire()
{
	bCanFirePistol = true;
}

void ANetPlayer::PistolReload()
{
	bIsReloadingPistol = true;
}

void ANetPlayer::PlayOnServer_Implementation(UAudioComponent* ComponentToPlay)
{
	PlayOnClients(ComponentToPlay);
}

void ANetPlayer::PlayOnClients_Implementation(UAudioComponent* ComponentToPlay)
{
	ComponentToPlay->Play(0.f);
}

void ANetPlayer::OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(TEXT("JumpReset")))
	{
		JumpsLeft = JumpMaxCount;
		MovementAudioComponent->SetSound(LandSound);
		PlayOnServer(MovementAudioComponent);
	}
}

void ANetPlayer::OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}