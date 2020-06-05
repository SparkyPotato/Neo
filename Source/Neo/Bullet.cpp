// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsActivated = false;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActivated)
	{
		FVector UnitVector = TargetVector - GetActorLocation();
		UnitVector.Normalize(1.f);
		SetActorLocation(UnitVector * Speed * DeltaTime + GetActorLocation());
	}
}

void ABullet::Activate(FVector Vector)
{
	TargetVector = Vector;
	bIsActivated = true;
	GetWorld()->GetTimerManager().SetTimer(KillTimer, this, &ABullet::Kill, 1.f, false);
}

void ABullet::Kill()
{
	Destroy();
}