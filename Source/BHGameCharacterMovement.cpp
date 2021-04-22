#include "BHGameCharacterMovement.h"
#include "BHGameCharacter.h"

//----------------------------------------------------------------------//
// UPawnMovementComponent
//----------------------------------------------------------------------//
UBHGameCharacterMovement::UBHGameCharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

float UBHGameCharacterMovement::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const ABHGameCharacter* BHGameCharacterOwner = Cast<ABHGameCharacter>(PawnOwner);
	if (BHGameCharacterOwner)
	{
		if (BHGameCharacterOwner->IsTargeting())
		{
			MaxSpeed *= BHGameCharacterOwner->GetTargetingSpeedModifier();
		}
		if (BHGameCharacterOwner->IsRunning())
		{
			MaxSpeed *= BHGameCharacterOwner->GetRunningSpeedModifier();
		}
	}

	return MaxSpeed;
}