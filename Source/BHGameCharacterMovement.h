#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "BHGameCharacterMovement.generated.h"

UCLASS()
class UBHGameCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()

	virtual float GetMaxSpeed() const override;
};
