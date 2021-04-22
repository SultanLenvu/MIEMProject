#include "BHGamePlayerCameraManager.h"
#include "BHGameCharacter.h"
#include "BHGameWeapon_Instant.h"

ABHGamePlayerCameraManager::ABHGamePlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NormalFOV = 90.0f;
	TargetingFOV = 60.0f;
	ViewPitchMin = -87.0f;
	ViewPitchMax = 87.0f;
	bAlwaysApplyModifiers = true;
}

void ABHGamePlayerCameraManager::UpdateCamera(float DeltaTime)
{
	ABHGameCharacter* MyPawn = PCOwner ? Cast<ABHGameCharacter>(PCOwner->GetPawn()) : NULL;
	if (MyPawn && MyPawn->IsFirstPerson())
	{
		const float TargetFOV = MyPawn->IsTargeting() ? TargetingFOV : NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 20.0f);
		/*if (MyPawn->IsTargeting())
		{
			PCOwner->SetViewTarget(Cast<ABHGameWeapon_Instant>(MyPawn->GetWeapon()));
		}
		else
		{
			PCOwner->SetViewTarget(MyPawn);
		}*/
	}

	Super::UpdateCamera(DeltaTime);

	if (MyPawn && MyPawn->IsFirstPerson())
	{
		MyPawn->OnCameraUpdate(GetCameraLocation(), GetCameraRotation());
	}
}

