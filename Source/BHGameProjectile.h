#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BHGameProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class ABHGameProjectile : public AActor
{
	GENERATED_BODY()

public: 
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* ProjectileSphereComponent;

	// Static Mesh 
	class UStaticMeshComponent* ProjectileMesh;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovementComponent;

	// Particle used when the projectile impacts against another object and explodes
	UPROPERTY(EditAnywhere, Category = "Effects")
		class UParticleSystem* ExplosionEffect;

	// The damage type and damage that will be done by this projectile
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		TSubclassOf<class UDamageType> DamageType;

	// The damage dealt by this projectile 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float Damage;

public:
	ABHGameProjectile();

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return ProjectileSphereComponent; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovementComponent; }

protected: 
	virtual void Destroyed() override;

	/** called when projectile hits something */
	UFUNCTION(Category = "Projectile")
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};

