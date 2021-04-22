#include "BHGameWeapon.h"
#include "BHGameCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "BHGamePlayerState.h"
#include "BHGameHUD.h"
#include "Camera/CameraShake.h"
#include "BHGamePlayerController.h"
#include "BHGame/Customization/PlayerWeaponService.h"
#include "BHGame/Customization/ModuleWeaponService.h"
#include "Engine/AssetManager.h"
#include "BHGame.h"

ABHGameWeapon::ABHGameWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Frame = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Frame"));
	RootComponent = Frame;

	bLoopedMuzzleFX = false;
	bLoopedFireAnim = false;
	bPlayingFireAnim = false;
	bIsEquipped = false;
	bWantsToFire = false;
	bPendingReload = false;
	bPendingEquip = false;
	CurrentState = EWeaponState::Idle;

	MuzzelComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(Frame, TEXT("Muzzel"));
	SetupSkeletalMeshComponent(MuzzelComp);

	MuzzelComp->SetupAttachment(Frame, TEXT("muzzel"));

	ForeGripComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(Frame, TEXT("ForeGrip"));
	SetupSkeletalMeshComponent(ForeGripComp);
	ForeGripComp->SetupAttachment(Frame, TEXT("foregrip"));

	StockComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(Frame, TEXT("Stock"));
	SetupSkeletalMeshComponent(StockComp);
	StockComp->SetupAttachment(Frame, TEXT("stock"));

	HandleComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(Frame, TEXT("Handle"));
	SetupSkeletalMeshComponent(HandleComp);
	HandleComp->SetupAttachment(Frame, TEXT("handle"));

	GripComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(ForeGripComp, TEXT("Grip"));
	SetupSkeletalMeshComponent(GripComp);
	if (ForeGripComp->SkeletalMesh) GripComp->SetupAttachment(ForeGripComp, TEXT("grip"));

	GunPointComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(MuzzelComp, TEXT("GunPoint"));
	SetupSkeletalMeshComponent(GunPointComp);

	CurrentAmmo = 0;
	CurrentAmmoInClip = 0;
	BurstCounter = 0;
	LastFireTime = 0.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
}

void ABHGameWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponConfig.InitialClips > 0)
	{
		CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
		CurrentAmmo = WeaponConfig.AmmoPerClip * WeaponConfig.InitialClips;
	}

	DetachMeshFromPawn();
}

void ABHGameWeapon::Destroyed()
{
	Super::Destroyed();

	StopSimulatingWeaponFire();
	if (MuzzelHandle.IsValid()) MuzzelHandle->ReleaseHandle();
	if (ForeGripHandle.IsValid()) ForeGripHandle->ReleaseHandle();
	if (StockHandle.IsValid()) StockHandle->ReleaseHandle();
	if (HandleHandle.IsValid()) HandleHandle->ReleaseHandle();
	if (GripHandle.IsValid()) GripHandle->ReleaseHandle();
	if (GunPointHandle.IsValid()) GunPointHandle->ReleaseHandle();
}

//////////////////////////////////////////////////////////////////////////
// Inventory

void ABHGameWeapon::OnEquip(const ABHGameWeapon* LastWeapon)
{
	AttachMeshToPawn();

	bPendingEquip = true;
	DetermineWeaponState();

	// Only play animation if last weapon is valid
	if (LastWeapon)
	{
		float Duration = PlayWeaponAnimation(EquipAnim);
		if (Duration <= 0.0f)
		{
			// failsafe
			Duration = 0.5f;
		}
		EquipStartedTime = GetWorld()->GetTimeSeconds();
		EquipDuration = Duration;

		GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &ABHGameWeapon::OnEquipFinished, Duration, false);
	}
	else
	{
		OnEquipFinished();
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		PlayWeaponSound(EquipSound);
	}

	ABHGameCharacter::NotifyEquipWeapon.Broadcast(MyPawn, this);
}

void ABHGameWeapon::OnEquipFinished()
{
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;

	// Determine the state so that the can reload checks will work
	DetermineWeaponState();

	if (MyPawn)
	{
		// try to reload empty clip
		if (MyPawn->IsLocallyControlled() &&
			CurrentAmmoInClip <= 0 &&
			CanReload())
		{
			StartReload();
		}
	}


}

void ABHGameWeapon::OnUnEquip()
{
	DetachMeshFromPawn();
	bIsEquipped = false;
	StopFire();

	if (bPendingReload)
	{
		StopWeaponAnimation(ReloadAnim);
		bPendingReload = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}

	if (bPendingEquip)
	{
		StopWeaponAnimation(EquipAnim);
		bPendingEquip = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	}

	ABHGameCharacter::NotifyUnEquipWeapon.Broadcast(MyPawn, this);

	DetermineWeaponState();
}

void ABHGameWeapon::OnEnterInventory(ABHGameCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
}

void ABHGameWeapon::OnLeaveInventory()
{
	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		SetOwningPawn(NULL);
	}
}

void ABHGameWeapon::AttachMeshToPawn()
{
	if (MyPawn)
	{
		// Remove and hide both first and third person meshes
		DetachMeshFromPawn();

		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		FName AttachPoint = MyPawn->GetWeaponAttachPoint();
		if (MyPawn->IsLocallyControlled() == true)
		{
			USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecifcPawnMesh(true);
			Frame->SetHiddenInGame(false,true);
			Frame->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		}
		else
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			UseWeaponMesh->SetHiddenInGame(false,true);
		}
	}
}

void ABHGameWeapon::DetachMeshFromPawn()
{
	Frame->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Frame->SetHiddenInGame(true,true);
}


//////////////////////////////////////////////////////////////////////////
// Input

void ABHGameWeapon::StartFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStartFire();
	}

	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

void ABHGameWeapon::StopFire()
{
	if ((GetLocalRole() < ROLE_Authority) && MyPawn && MyPawn->IsLocallyControlled())
	{
		ServerStopFire();
	}

	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

void ABHGameWeapon::StartReload(bool bFromReplication)
{
	if (!bFromReplication && GetLocalRole() < ROLE_Authority)
	{
		ServerStartReload();
	}

	if (bFromReplication || CanReload())
	{
		bPendingReload = true;
		DetermineWeaponState();

		float AnimDuration = PlayWeaponAnimation(ReloadAnim);
		if (AnimDuration <= 0.0f)
		{
			AnimDuration = WeaponConfig.NoAnimReloadDuration;
		}

		GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &ABHGameWeapon::StopReload, AnimDuration, false);
		if (GetLocalRole() == ROLE_Authority)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ABHGameWeapon::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			PlayWeaponSound(ReloadSound);
		}
	}
}

void ABHGameWeapon::StopReload()
{
	if (CurrentState == EWeaponState::Reloading)
	{
		bPendingReload = false;
		DetermineWeaponState();
		StopWeaponAnimation(ReloadAnim);
	}
}

bool ABHGameWeapon::ServerStartFire_Validate()
{
	return true;
}

void ABHGameWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

bool ABHGameWeapon::ServerStopFire_Validate()
{
	return true;
}

void ABHGameWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

bool ABHGameWeapon::ServerStartReload_Validate()
{
	return true;
}

void ABHGameWeapon::ServerStartReload_Implementation()
{
	StartReload();
}

bool ABHGameWeapon::ServerStopReload_Validate()
{
	return true;
}

void ABHGameWeapon::ServerStopReload_Implementation()
{
	StopReload();
}

void ABHGameWeapon::ClientStartReload_Implementation()
{
	StartReload();
}

//////////////////////////////////////////////////////////////////////////
// Control

bool ABHGameWeapon::CanFire() const
{
	bool bCanFire = MyPawn && MyPawn->CanFire();
	bool bStateOKToFire = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanFire == true) && (bStateOKToFire == true) && (bPendingReload == false));
}

bool ABHGameWeapon::CanReload() const
{
	bool bCanReload = (!MyPawn || MyPawn->CanReload());
	bool bGotAmmo = (CurrentAmmoInClip < WeaponConfig.AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0 || HasInfiniteClip());
	bool bStateOKToReload = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanReload == true) && (bGotAmmo == true) && (bStateOKToReload == true));
}


//////////////////////////////////////////////////////////////////////////
// Weapon usage

void ABHGameWeapon::GiveAmmo(int AddAmount)
{
	const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);
	AddAmount = FMath::Min(AddAmount, MissingAmmo);
	CurrentAmmo += AddAmount;

	/*ABHGameAIController* BotAI = MyPawn ? Cast<ABHGameAIController>(MyPawn->GetController()) : NULL;
	if (BotAI)
	{
		BotAI->CheckAmmo(this);
	}*/

	// start reload if clip was empty
	if (GetCurrentAmmoInClip() <= 0 &&
		CanReload() &&
		MyPawn && (MyPawn->GetWeapon() == this))
	{
		ClientStartReload();
	}
}

void ABHGameWeapon::UseAmmo()
{
	if (!HasInfiniteAmmo())
	{
		CurrentAmmoInClip--;
	}

	if (!HasInfiniteAmmo() && !HasInfiniteClip())
	{
		CurrentAmmo--;
	}

	ABHGamePlayerController* PlayerController = MyPawn ? Cast<ABHGamePlayerController>(MyPawn->GetController()) : NULL;
	/*ABHGameAIController* BotAI = MyPawn ? Cast<ABHGameAIController>(MyPawn->GetController()) : NULL;
	
	if (BotAI)
	{
		BotAI->CheckAmmo(this);
	}
	else*/ if (PlayerController)
	{
		ABHGamePlayerState* PlayerState = Cast<ABHGamePlayerState>(PlayerController->PlayerState);
		switch (GetAmmoType())
		{
		case EAmmoType::ERocket:
			//PlayerState->AddRocketsFired(1);
			break;
		case EAmmoType::EBullet:
		default:
			//PlayerState->AddBulletsFired(1);
			break;
		}
	}
}

void ABHGameWeapon::HandleReFiring()
{
	// Update TimerIntervalAdjustment
	UWorld* MyWorld = GetWorld();

	float SlackTimeThisFrame = FMath::Max(0.0f, (MyWorld->TimeSeconds - LastFireTime) - WeaponConfig.TimeBetweenShots);

	if (bAllowAutomaticWeaponCatchup)
	{
		TimerIntervalAdjustment -= SlackTimeThisFrame;
	}

	HandleFiring();
}

void ABHGameWeapon::HandleFiring()
{
	if ((CurrentAmmoInClip > 0 || HasInfiniteClip() || HasInfiniteAmmo()) && CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			FireWeapon();

			UseAmmo();

			// update firing FX on remote clients if function was called on server
			BurstCounter++;
		}
	}
	else if (CanReload())
	{
		StartReload();
	}
	else if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (GetCurrentAmmo() == 0 && !bRefiring)
		{
			PlayWeaponSound(OutOfAmmoSound);
			ABHGamePlayerController* MyPC = Cast<ABHGamePlayerController>(MyPawn->Controller);
			//ABHGameHUD* MyHUD = MyPC ? Cast<ABHGameHUD>(MyPC->GetHUD()) : NULL;
			//if (MyHUD)
			//{
			//	MyHUD->NotifyOutOfAmmo();
			//}
		}

		// stop weapon fire FX, but stay in Firing state
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}
	else
	{
		OnBurstFinished();
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// local client will notify server
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerHandleFiring();
		}

		// reload after firing last round
		if (CurrentAmmoInClip <= 0 && CanReload())
		{
			StartReload();
		}

		// setup refire timer
		bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABHGameWeapon::HandleReFiring, FMath::Max<float>(WeaponConfig.TimeBetweenShots + TimerIntervalAdjustment, SMALL_NUMBER), false);
			TimerIntervalAdjustment = 0.f;
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

bool ABHGameWeapon::ServerHandleFiring_Validate()
{
	return true;
}

void ABHGameWeapon::ServerHandleFiring_Implementation()
{
	const bool bShouldUpdateAmmo = (CurrentAmmoInClip > 0 && CanFire());

	HandleFiring();

	if (bShouldUpdateAmmo)
	{
		// update ammo
		UseAmmo();

		// update firing FX on remote clients
		BurstCounter++;
	}
}

void ABHGameWeapon::ReloadWeapon()
{
	int32 ClipDelta = FMath::Min(WeaponConfig.AmmoPerClip - CurrentAmmoInClip, CurrentAmmo - CurrentAmmoInClip);

	if (HasInfiniteClip())
	{
		ClipDelta = WeaponConfig.AmmoPerClip - CurrentAmmoInClip;
	}

	if (ClipDelta > 0)
	{
		CurrentAmmoInClip += ClipDelta;
	}

	if (HasInfiniteClip())
	{
		CurrentAmmo = FMath::Max(CurrentAmmoInClip, CurrentAmmo);
	}
}

void ABHGameWeapon::SetWeaponState(EWeaponState::Type NewState)
{
	const EWeaponState::Type PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}
}

void ABHGameWeapon::DetermineWeaponState()
{
	EWeaponState::Type NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if (bPendingReload)
		{
			if (CanReload() == false)
			{
				NewState = CurrentState;
			}
			else
			{
				NewState = EWeaponState::Reloading;
			}
		}
		else if ((bPendingReload == false) && (bWantsToFire == true) && (CanFire() == true))
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}

	SetWeaponState(NewState);
}

void ABHGameWeapon::OnBurstStarted()
{
	// start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && WeaponConfig.TimeBetweenShots > 0.0f &&
		LastFireTime + WeaponConfig.TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABHGameWeapon::HandleFiring, LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}

void ABHGameWeapon::OnBurstFinished()
{
	// stop firing FX on remote clients
	BurstCounter = 0;

	// stop firing FX locally, unless it's a dedicated server
	//if (GetNetMode() != NM_DedicatedServer)
	//{
	StopSimulatingWeaponFire();
	//}

	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	bRefiring = false;

	// reset firing interval adjustment
	TimerIntervalAdjustment = 0.0f;
}


//////////////////////////////////////////////////////////////////////////
// Weapon usage helpers

UAudioComponent* ABHGameWeapon::PlayWeaponSound(USoundCue* Sound)
{
	UAudioComponent* AC = NULL;
	if (Sound && MyPawn)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, MyPawn->GetRootComponent());
	}

	return AC;
}

float ABHGameWeapon::PlayWeaponAnimation(const FWeaponAnim& Animation)
{
	float Duration = 0.0f;
	if (MyPawn)
	{
		UAnimMontage* UseAnim = MyPawn->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		if (UseAnim)
		{
			Duration = MyPawn->PlayAnimMontage(UseAnim);
		}
		
	}

	return Duration;
}

void ABHGameWeapon::StopWeaponAnimation(const FWeaponAnim& Animation)
{
	if (MyPawn)
	{
		UAnimMontage* UseAnim = MyPawn->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		if (UseAnim)
		{
			MyPawn->StopAnimMontage(UseAnim);
		}
	}
}

FVector ABHGameWeapon::GetCameraAim() const
{
	ABHGamePlayerController* const PlayerController = GetInstigatorController<ABHGamePlayerController>();
	FVector FinalAim = FVector::ZeroVector;

	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (GetInstigator())
	{
		FinalAim = GetInstigator()->GetBaseAimRotation().Vector();
	}

	return FinalAim;
}

void ABHGameWeapon::SetupModulesServer_Implementation(const TArray<FName>& Paths,
	const TArray<FName>& SocketNameArray)
{
	for (uint8 index = 0; index < Paths.Num(); index++)
	{
		this->PathArray.Add(Paths[index]);
	}

	if (GetNetMode() != NM_DedicatedServer)
	{
		SetupModuleMeshes(PathArray);
	}
}
bool ABHGameWeapon::SetupModulesServer_Validate(const TArray<FName>& Paths, const TArray<FName>& SocketNameArray)
{
	return true;
}

void ABHGameWeapon::ExtractModulesData()
{
	UPlayerWeaponService* WPService = NewObject<UPlayerWeaponService>();
	TArray<FPlayerWeaponDataRow*> Weapons;
	WPService->GetWeapons(Weapons);
	UModuleWeaponService* MService = NewObject<UModuleWeaponService>();
	auto FrameModule = MService->GetFrame(Weapons[0]->Frame);
	auto ForeGripModule = MService->GetForeGrip(Weapons[0]->ForeGrip);
	auto StockModule = MService->GetStock(Weapons[0]->Stock);
	auto MuzzelModule = MService->GetMuzzel(Weapons[0]->Muzzel);
	auto HandleModule = MService->GetHandle(Weapons[0]->Handle);
	auto MagazineModule = MService->GetMagazine(Weapons[0]->Magazine);
	auto ScopeModule = MService->GetScope(Weapons[0]->Scope);
	auto GripModule = MService->GetGrip(Weapons[0]->Grip);
	auto GunPointModule = MService->GetGunPoint(Weapons[0]->GunPoint);
	TArray<FName> Paths = TArray<FName>();
	TArray<FName> SocketNameArray = TArray<FName>();
	Paths.Add(MuzzelModule->AssetPath); SocketNameArray.Add(FrameModule->MuzzelSocketName);
	Paths.Add(ForeGripModule->AssetPath); SocketNameArray.Add(FrameModule->ForeGripSocketName);
	Paths.Add(StockModule->AssetPath); SocketNameArray.Add(FrameModule->StockSocketName);
	Paths.Add(HandleModule->AssetPath); SocketNameArray.Add(FrameModule->HandleSocketName);
	Paths.Add(MagazineModule->AssetPath); SocketNameArray.Add(FrameModule->MagazineSocketName);
	Paths.Add(ScopeModule->AssetPath); SocketNameArray.Add(FrameModule->ScopeSocketName);
	Paths.Add(GripModule->AssetPath); SocketNameArray.Add(ForeGripModule->GripSocketName);
	Paths.Add(GunPointModule->AssetPath); SocketNameArray.Add(MuzzelModule->GunPointSocketName);
	SetupModulesServer(Paths, SocketNameArray);
}

FVector ABHGameWeapon::GetAdjustedAim() const
{
	ABHGamePlayerController* const PlayerController = GetInstigatorController<ABHGamePlayerController>();
	FVector FinalAim = FVector::ZeroVector;
	// If we have a player controller use it for the aim
	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	//else if (GetInstigator())
	//{
	//	// Now see if we have an AI controller - we will want to get the aim from there if we do
	//	ABHGameAIController* AIController = MyPawn ? Cast<ABHGameAIController>(MyPawn->Controller) : NULL;
	//	if (AIController != NULL)
	//	{
	//		FinalAim = AIController->GetControlRotation().Vector();
	//	}
	//	else
	//	{
	//		FinalAim = GetInstigator()->GetBaseAimRotation().Vector();
	//	}
	//}

	return FinalAim;
}

FVector ABHGameWeapon::GetCameraDamageStartLocation(const FVector& AimDir) const
{
	ABHGamePlayerController* PC = MyPawn ? Cast<ABHGamePlayerController>(MyPawn->Controller) : NULL;
	//ABHGameAIController* AIPC = MyPawn ? Cast<ABHGameAIController>(MyPawn->Controller) : NULL;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PC)
	{
		// use player's camera
		FRotator UnusedRot;
		PC->GetPlayerViewPoint(OutStartTrace, UnusedRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		OutStartTrace = OutStartTrace + AimDir * ((GetInstigator()->GetActorLocation() - OutStartTrace) | AimDir);
	}
	//else if (AIPC)
	//{
	//	OutStartTrace = GetMuzzleLocation();
	//}

	return OutStartTrace;
}

FVector ABHGameWeapon::GetMuzzleLocation()
{
	return GetMuzzelEdgeComponent()->GetSocketLocation(MuzzleAttachPoint);
}

FVector ABHGameWeapon::GetMuzzleDirection()
{
	return GetMuzzelEdgeComponent()->GetSocketRotation(MuzzleAttachPoint).Vector();
}

FHitResult ABHGameWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	return Hit;
}

void ABHGameWeapon::BeginPlay()
{
	if (GetNetMode()==NM_Client && MyPawn && MyPawn->IsLocallyControlled()) ExtractModulesData();
}

void ABHGameWeapon::SetOwningPawn(ABHGameCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		SetInstigator(NewOwner);
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);
	}
}

//////////////////////////////////////////////////////////////////////////
// Replication & effects

void ABHGameWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		OnLeaveInventory();
	}
}

void ABHGameWeapon::OnRep_BurstCounter()
{
	if (BurstCounter > 0)
	{
		SimulateWeaponFire();
	}
	else
	{
		StopSimulatingWeaponFire();
	}
}

void ABHGameWeapon::OnRep_Reload()
{
	if (bPendingReload)
	{
		StartReload(true);
	}
	else
	{
		StopReload();
	}
}

void ABHGameWeapon::SimulateWeaponFire()
{
	if (GetLocalRole() == ROLE_Authority && CurrentState != EWeaponState::Firing)
	{
		return;
	}

	if (MuzzleFX)
	{
		USkeletalMeshComponent* UseWeaponMesh = GetMuzzelEdgeComponent();
		if (!bLoopedMuzzleFX || MuzzlePSC == NULL)
		{
			// Split screen requires we create 2 effects. One that we see and one that the other player sees.
			if ((MyPawn != NULL) && (MyPawn->IsLocallyControlled() == true))
			{
				AController* PlayerCon = MyPawn->GetController();
				if (PlayerCon != NULL)
				{
					//Frame->GetSocketLocation(MuzzleAttachPoint);
					MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, MuzzelComp, MuzzleAttachPoint);
				}
			}
			else
			{
				MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, MuzzelComp, MuzzleAttachPoint);
			}
		}
	}

	if (!bLoopedFireAnim || !bPlayingFireAnim)
	{
		PlayWeaponAnimation(FireAnim);
		Frame->AnimScriptInstance->Montage_Play(ZatvorAnim);
		bPlayingFireAnim = true;
		
	}

	if (bLoopedFireSound)
	{
		if (FireAC == NULL)
		{
			FireAC = PlayWeaponSound(FireLoopSound);
		}
	}
	else
	{
		PlayWeaponSound(FireSound);
	}

	ABHGamePlayerController* PC = (MyPawn != NULL) ? Cast<ABHGamePlayerController>(MyPawn->Controller) : NULL;
	if (PC != NULL && PC->IsLocalController())
	{
		if (FireCameraShake != NULL)
		{
			PC->ClientStartCameraShake(FireCameraShake, 1);
		}
		if (FireForceFeedback != NULL/* && PC->IsVibrationEnabled()*/)
		{
			FForceFeedbackParameters FFParams;
			FFParams.Tag = "Weapon";
			PC->ClientPlayForceFeedback(FireForceFeedback, FFParams);
		}
	}
}

void ABHGameWeapon::StopSimulatingWeaponFire()
{
	if (bLoopedMuzzleFX)
	{
		if (MuzzlePSC != NULL)
		{
			MuzzlePSC->DeactivateSystem();
			MuzzlePSC = NULL;
		}
		if (MuzzlePSCSecondary != NULL)
		{
			MuzzlePSCSecondary->DeactivateSystem();
			MuzzlePSCSecondary = NULL;
		}
	}

	if (bLoopedFireAnim && bPlayingFireAnim)
	{
		StopWeaponAnimation(FireAnim);
		if (Frame->AnimScriptInstance->Montage_IsPlaying(ZatvorAnim))
			Frame->AnimScriptInstance->Montage_Stop(ZatvorAnim->BlendOut.GetBlendTime(), ZatvorAnim);
		bPlayingFireAnim = false;
	}

	if (FireAC)
	{
		FireAC->FadeOut(0.1f, 0.0f);
		FireAC = NULL;

		PlayWeaponSound(FireFinishSound);
	}
}

void ABHGameWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABHGameWeapon, MyPawn);
	DOREPLIFETIME(ABHGameWeapon, PathArray);
	DOREPLIFETIME_CONDITION(ABHGameWeapon, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABHGameWeapon, CurrentAmmoInClip, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ABHGameWeapon, BurstCounter, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABHGameWeapon, bPendingReload, COND_SkipOwner);
}

USkeletalMeshComponent* ABHGameWeapon::GetWeaponMesh() const
{
	 return Frame;
}

class ABHGameCharacter* ABHGameWeapon::GetPawnOwner() const
{
	return MyPawn;
}

bool ABHGameWeapon::IsEquipped() const
{
	return bIsEquipped;
}

bool ABHGameWeapon::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}

EWeaponState::Type ABHGameWeapon::GetCurrentState() const
{
	return CurrentState;
}

int32 ABHGameWeapon::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

int32 ABHGameWeapon::GetCurrentAmmoInClip() const
{
	return CurrentAmmoInClip;
}

int32 ABHGameWeapon::GetAmmoPerClip() const
{
	return WeaponConfig.AmmoPerClip;
}

int32 ABHGameWeapon::GetMaxAmmo() const
{
	return WeaponConfig.MaxAmmo;
}

bool ABHGameWeapon::HasInfiniteAmmo() const
{
	const ABHGamePlayerController* MyPC = (MyPawn != NULL) ? Cast<const ABHGamePlayerController>(MyPawn->Controller) : NULL;
	return WeaponConfig.bInfiniteAmmo || MyPC;
}

bool ABHGameWeapon::HasInfiniteClip() const
{
	const ABHGamePlayerController* MyPC = (MyPawn != NULL) ? Cast<const ABHGamePlayerController>(MyPawn->Controller) : NULL;
	return WeaponConfig.bInfiniteClip || MyPC;
}

float ABHGameWeapon::GetEquipStartedTime() const
{
	return EquipStartedTime;
}

float ABHGameWeapon::GetEquipDuration() const
{
	return EquipDuration;
}

void ABHGameWeapon::OnMeshLoaded(USkeletalMeshComponent* Component, TSharedPtr<FStreamableHandle>* MeshHandle)
{
	USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshHandle->Get()->GetLoadedAsset());
	Component->SetSkeletalMesh(LoadedMesh);
}

USkeletalMeshComponent* ABHGameWeapon::GetMuzzelEdgeComponent()
{
	return MuzzelComp;
}

void ABHGameWeapon::OnRep_PathArray(const TArray<FName>& PArray)
{
	SetupModuleMeshes(PArray);
}

void ABHGameWeapon::SetupModuleMeshes(const TArray<FName>& Paths)
{
	if (PathArray.Num() == 0) return;
	UAssetManager& AssetManager = UAssetManager::Get();
	FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();

	if (MuzzelHandle.IsValid())
	{
		if (!MuzzelHandle->IsActive())
		{
			FStreamableDelegate MuzzelDelegate = FStreamableDelegate::CreateUObject(this, &ABHGameWeapon::OnMeshLoaded, MuzzelComp, &MuzzelHandle);
			MuzzelHandle = StreamableManager.RequestAsyncLoad(FSoftObjectPath(PathArray[0]), MuzzelDelegate);
			//MuzzelComp->AttachToComponent(Frame,FAttachmentTransformRules::KeepRelativeTransform,SocketNameArray[0]);
		}
	}
	else
	{
		FStreamableDelegate MuzzelDelegate = FStreamableDelegate::CreateUObject(this, &ABHGameWeapon::OnMeshLoaded, MuzzelComp, &MuzzelHandle);
		MuzzelHandle = StreamableManager.RequestAsyncLoad(FSoftObjectPath(PathArray[0]), MuzzelDelegate);
	}

	if (ForeGripHandle.IsValid())
	{
		if (!ForeGripHandle->IsActive())
		{
			FStreamableDelegate ForeGripDelegate = FStreamableDelegate::CreateUObject(this, &ABHGameWeapon::OnMeshLoaded, ForeGripComp, &ForeGripHandle);
			ForeGripHandle = StreamableManager.RequestAsyncLoad(FSoftObjectPath(PathArray[1]), ForeGripDelegate);
			//ForeGripComp->AttachToComponent(Frame,FAttachmentTransformRules::KeepRelativeTransform,SocketNameArray[1]);
		}
	}
	else
	{
		FStreamableDelegate ForeGripDelegate = FStreamableDelegate::CreateUObject(this, &ABHGameWeapon::OnMeshLoaded, ForeGripComp, &ForeGripHandle);
		ForeGripHandle = StreamableManager.RequestAsyncLoad(FSoftObjectPath(PathArray[1]), ForeGripDelegate);
	}

	if (StockHandle.IsValid())
	{
		if (!StockHandle->IsActive())
		{
			FStreamableDelegate StockDelegate = FStreamableDelegate::CreateUObject(this, &ABHGameWeapon::OnMeshLoaded, StockComp, &StockHandle);
			StockHandle = StreamableManager.RequestAsyncLoad(FSoftObjectPath(PathArray[2]), StockDelegate);
			//StockComp->AttachToComponent(Frame,FAttachmentTransformRules::KeepRelativeTransform,SocketNameArray[2]);
		}
	}
	else
	{
		FStreamableDelegate StockDelegate = FStreamableDelegate::CreateUObject(this, &ABHGameWeapon::OnMeshLoaded, StockComp, &StockHandle);
		StockHandle = StreamableManager.RequestAsyncLoad(FSoftObjectPath(PathArray[2]), StockDelegate);
	}

	if (HandleHandle.IsValid())
	{
		if (!HandleHandle->IsActive())
		{
			FStreamableDelegate HandleDelegate = FStreamableDelegate::CreateUObject(this, &ABHGameWeapon::OnMeshLoaded, HandleComp, &HandleHandle);
			HandleHandle = StreamableManager.RequestAsyncLoad(FSoftObjectPath(PathArray[3]), HandleDelegate);
			//HandleComp->AttachToComponent(Frame,FAttachmentTransformRules::KeepRelativeTransform,SocketNameArray[3]);
		}
	}
	else
	{
		FStreamableDelegate HandleDelegate = FStreamableDelegate::CreateUObject(this, &ABHGameWeapon::OnMeshLoaded, HandleComp, &HandleHandle);
		HandleHandle = StreamableManager.RequestAsyncLoad(FSoftObjectPath(PathArray[3]), HandleDelegate);
	}

	if (!PathArray[4].IsNone() || Paths.Num() == 0)
	{
		UObject* loadedObject = StaticLoadObject(UObject::StaticClass(), nullptr, *(PathArray[4].ToString()));
		UBlueprint* castedBlueprint = Cast<UBlueprint>(loadedObject);
		if (castedBlueprint && castedBlueprint->GeneratedClass->IsChildOf(ASkeletalMeshActor::StaticClass()))
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParameters.Instigator = MyPawn;
			MagazineComp = GetWorld()->SpawnActor<ASkeletalMeshActor>(castedBlueprint->GeneratedClass, SpawnParameters);
			if (MagazineComp->IsAttachedTo(this)) MagazineComp->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			MagazineComp->AttachToComponent(Frame, FAttachmentTransformRules::KeepRelativeTransform, TEXT("magazine"));
			MagazineClass = castedBlueprint->GeneratedClass;
		}
	}

	if (!PathArray[5].IsNone() || Paths.Num() == 0)
	{
		UObject* loadedObject = StaticLoadObject(UObject::StaticClass(), nullptr, *(PathArray[5].ToString()));
		UBlueprint* castedBlueprint = Cast<UBlueprint>(loadedObject);
		if (castedBlueprint && castedBlueprint->GeneratedClass->IsChildOf(AScopeBase::StaticClass()))
		{
			FActorSpawnParameters SpawnParameters;
			//SpawnParameters.Owner=this;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParameters.Instigator = MyPawn;
			ScopeComp = GetWorld()->SpawnActor<AScopeBase>(castedBlueprint->GeneratedClass, SpawnParameters);
			if (ScopeComp->IsAttachedTo(this)) ScopeComp->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			ScopeComp->AttachToComponent(Frame, FAttachmentTransformRules::KeepRelativeTransform, TEXT("scope"));
		}
	}

	if (GripHandle.IsValid())
	{
		if (!GripHandle->IsActive())
		{
			if (!PathArray[6].IsNone())
			{
				FStreamableDelegate GripDelegate = FStreamableDelegate::CreateUObject(this, &ABHGameWeapon::OnMeshLoaded, GripComp, &GripHandle);
				GripHandle = StreamableManager.RequestAsyncLoad(FSoftObjectPath(PathArray[6]), GripDelegate);
			}

		}
	}
	else
	{
		if (!PathArray[6].IsNone())
		{
			FStreamableDelegate GripDelegate = FStreamableDelegate::CreateUObject(this, &ABHGameWeapon::OnMeshLoaded, GripComp, &GripHandle);
			GripHandle = StreamableManager.RequestAsyncLoad(FSoftObjectPath(PathArray[6]), GripDelegate);
		}
		GripComp->AttachToComponent(ForeGripComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("grip"));
	}

	if (GunPointHandle.IsValid())
	{
		if (!GunPointHandle->IsActive())
		{
			if (!PathArray[7].IsNone())
			{
				FStreamableDelegate GunPointDelegate = FStreamableDelegate::CreateUObject(this, &ABHGameWeapon::OnMeshLoaded, GunPointComp, &GunPointHandle);
				GunPointHandle = StreamableManager.RequestAsyncLoad(FSoftObjectPath(PathArray[7]), GunPointDelegate);
			}
		}
	}
	else
	{
		if (!PathArray[7].IsNone())
		{
			FStreamableDelegate GunPointDelegate = FStreamableDelegate::CreateUObject(this, &ABHGameWeapon::OnMeshLoaded, GunPointComp, &GunPointHandle);
			GunPointHandle = StreamableManager.RequestAsyncLoad(FSoftObjectPath(PathArray[7]), GunPointDelegate);
			GunPointComp->AttachToComponent(MuzzelComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("gunpoint"));
		}
	}
}

void ABHGameWeapon::SetupSkeletalMeshComponent(USkeletalMeshComponent*& Component)
{
	Component->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Component->bReceivesDecals = true;
	Component->CastShadow = true;
	Component->SetCollisionObjectType(ECC_WorldDynamic);
	Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Component->SetCollisionResponseToAllChannels(ECR_Ignore);
	Component->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	Component->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Component->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	Component->bOnlyOwnerSee = false;
}
