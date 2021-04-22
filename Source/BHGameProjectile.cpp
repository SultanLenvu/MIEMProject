// Copyright Epic Games, Inc. All Rights Reserved.

#include "BHGameProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "BHGameCharacter.h"

ABHGameProjectile::ABHGameProjectile() 
{
	// Use a sphere as a simple collision representation
	ProjectileSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	ProjectileSphereComponent->InitSphereRadius(5.0f);
	ProjectileSphereComponent->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileSphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	// Players can't walk on it
	ProjectileSphereComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	ProjectileSphereComponent->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = ProjectileSphereComponent;

	// Redistering the projectile impact on a hit event
	if (GetLocalRole() == ROLE_Authority)
	{
		ProjectileSphereComponent->OnComponentHit.AddDynamic(this, &ABHGameProjectile::OnHit);
	}

	// Mesh Definition
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/FirstPerson/Meshes/FirstPersonProjectileMesh.FirstPersonProjectileMesh"));
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	// Set the projectile mesh and its position/scale if we successfully found a mesh asset to use
	if (DefaultMesh.Succeeded())
	{
		ProjectileMesh->SetStaticMesh(DefaultMesh.Object);
		ProjectileMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -5.0f));
		ProjectileMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	}

	// Use a ProjectileMovementComponentComponent to govern this projectile's movement
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovementComponent->UpdatedComponent = ProjectileSphereComponent;
	ProjectileMovementComponent->InitialSpeed = 3000.f;
	ProjectileMovementComponent->MaxSpeed = 3000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	// Explosion on hit
	//static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	//if (DefaultExplosionEffect.Succeeded())
	//{
	//	ExplosionEffect = DefaultExplosionEffect.Object;
	//}

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	// Enable replication
	bReplicates = true;

	// Damage property
	DamageType = UDamageType::StaticClass();
	Damage = 10.0f;
}

void ABHGameProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ABHGameCharacter* Enemy = Cast<ABHGameCharacter>(OtherActor);
	if (Enemy != nullptr)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, nullptr, this, DamageType);	
	}
	Destroy();
}

void ABHGameProjectile::Destroyed()
{
	FVector spawnLocation = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
}