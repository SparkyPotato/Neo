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
}

void ANetPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	CollsionComponent = FindComponentByClass<UCapsuleComponent>();
	MeshComponent = FindComponentByClass<USkeletalMeshComponent>();
	MovementComponent = FindComponentByClass<UCharacterMovementComponent>();

	DetectionComponent->OnComponentBeginOverlap.AddDynamic(this, &ANetPlayer::OnDetectionOverlapBegin);
	DetectionComponent->OnComponentEndOverlap.AddDynamic(this, &ANetPlayer::OnDetectionOverlapEnd);

	JumpsLeft = JumpMaxCount;
	bCanPushoff = true;
	bCanDash = true;
	bCanUnCrouch = false;
	bTriedToUnCrouch = false;
}

void ANetPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
}

void ANetPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
		LaunchCharacter(FVector(0.f, 0.f, MovementComponent->JumpZVelocity), false, true);
		JumpsLeft--;
	}
}

void ANetPlayer::ClientCrouch()
{
	bCanUnCrouch = false;
	GetWorld()->GetTimerManager().SetTimer(UnCrouchTimer, this, &ANetPlayer::AllowUnCrouch, UnCrouchCooldown, false);
	Crouch();
	SetActorScale3D(FVector(1.f, 1.f, 0.5f));
	ServerCrouch();
	MovementComponent->SetWalkableFloorAngle(5.f);
	MovementComponent->BrakingFriction = 0.1f;
	MovementComponent->BrakingDecelerationWalking = 128.f;
	MovementComponent->GroundFriction = 0.f;
}

void ANetPlayer::ServerCrouch_Implementation()
{
	SetScale(FVector(1.f, 1.f, 0.5f));
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
	ServerUnCrouch();
	MovementComponent->SetWalkableFloorAngle(45.f);
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
	SetScale(FVector(1.f, 1.f, 1.f));
}

void ANetPlayer::SetScale_Implementation(FVector Scale)
{
	SetActorScale3D(Scale);
}

void ANetPlayer::ClientDash()
{
	if (bCanDash)
	{
		bCanDash = false;
		GetWorld()->GetTimerManager().SetTimer(DashTimer, this, &ANetPlayer::EndDash, DashCooldown, false);
		LaunchCharacter((FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X)) * MovementComponent->JumpZVelocity * 6, true, true);
	}
}

void ANetPlayer::EndDash()
{
	bCanDash = true;
}

void ANetPlayer::ClientPushoff()
{
	bCanPushoff = false;
	GetWorld()->GetTimerManager().SetTimer(PushoffTimer, this, &ANetPlayer::EndPushoff, PushoffCooldown, false);
	LaunchCharacter(FVector(0.f, 0.f, MovementComponent->JumpZVelocity * 3), true, true);
}

void ANetPlayer::EndPushoff()
{
	bCanPushoff = true;
}

void ANetPlayer::OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(TEXT("JumpReset")))
	{
		JumpsLeft = JumpMaxCount;
	}
}

void ANetPlayer::OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}