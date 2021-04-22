#pragma once

#include "BHGameGameMode.h"
//#include "BHGameGameInstance.h"
#include "BHGameHUD.h"
#include "BHGamePlayerState.h"
//#include "Online/BHGameGameSession.h"
//#include "BHGameTeamStart.h"
#include "BHGamePlayerController.h"

ABHGameGameMode::ABHGameGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/Blueprints/PlayerPawn"));
	DefaultPawnClass = PlayerPawnOb.Class;

	HUDClass = ABHGameHUD::StaticClass();
	PlayerControllerClass = ABHGamePlayerController::StaticClass();
	PlayerStateClass = ABHGamePlayerState::StaticClass();
	//GameStateClass = ABHGameGameState::StaticClass();

	MinRespawnDelay = 1.0f;

	bUseSeamlessTravel = FParse::Param(FCommandLine::Get(), TEXT("NoSeamlessTravel")) ? false : true;
}

//void ABHGameGameMode::PostInitProperties()
//{
//	Super::PostInitProperties();
//	if (PlatformPlayerControllerClass != nullptr)
//	{
//		PlayerControllerClass = PlatformPlayerControllerClass;
//	}
//}
//
//void ABHGameGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
//{
//	Super::InitGame(MapName, Options, ErrorMessage);
//
//	const UGameInstance* GameInstance = GetGameInstance();
//	if (GameInstance && Cast<UBHGameGameInstance>(GameInstance)->GetOnlineMode() != EOnlineMode::Offline)
//	{
//		bPauseable = false;
//	}
//}
//
///** Returns game session class to use */
//TSubclassOf<AGameSession> ABHGameGameMode::GetGameSessionClass() const
//{
//	return ABHGameGameSession::StaticClass();
//}
//
//void ABHGameGameMode::PreInitializeComponents()
//{
//	Super::PreInitializeComponents();
//
//	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &ABHGameGameMode::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
//}
//
//void ABHGameGameMode::DefaultTimer()
//{
//	// don't update timers for Play In Editor mode, it's not real match
//	if (GetWorld()->IsPlayInEditor())
//	{
//		// start match if necessary.
//		if (GetMatchState() == MatchState::WaitingToStart)
//		{
//			StartMatch();
//		}
//		return;
//	}
//
//	ABHGameGameState* const MyGameState = Cast<ABHGameGameState>(GameState);
//	if (MyGameState && MyGameState->RemainingTime > 0 && !MyGameState->bTimerPaused)
//	{
//		MyGameState->RemainingTime--;
//
//		if (MyGameState->RemainingTime <= 0)
//		{
//			if (GetMatchState() == MatchState::WaitingPostMatch)
//			{
//				RestartGame();
//			}
//			else if (GetMatchState() == MatchState::InProgress)
//			{
//				FinishMatch();
//
//				// Send end round events
//				for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
//				{
//					ABHGamePlayerController* PlayerController = Cast<ABHGamePlayerController>(*It);
//
//					if (PlayerController && MyGameState)
//					{
//						ABHGamePlayerState* PlayerState = Cast<ABHGamePlayerState>((*It)->PlayerState);
//						const bool bIsWinner = IsWinner(PlayerState);
//
//						PlayerController->ClientSendRoundEndEvent(bIsWinner, MyGameState->ElapsedTime);
//					}
//				}
//			}
//			else if (GetMatchState() == MatchState::WaitingToStart)
//			{
//				StartMatch();
//			}
//		}
//	}
//}
//
//void ABHGameGameMode::HandleMatchIsWaitingToStart()
//{
//	Super::HandleMatchIsWaitingToStart();
//
//	if (bDelayedStart)
//	{
//		// start warmup if needed
//		ABHGameGameState* const MyGameState = Cast<ABHGameGameState>(GameState);
//		if (MyGameState && MyGameState->RemainingTime == 0)
//		{
//			const bool bWantsMatchWarmup = !GetWorld()->IsPlayInEditor();
//			if (bWantsMatchWarmup && WarmupTime > 0)
//			{
//				MyGameState->RemainingTime = WarmupTime;
//			}
//			else
//			{
//				MyGameState->RemainingTime = 0.0f;
//			}
//		}
//	}
//}
//
//void ABHGameGameMode::HandleMatchHasStarted()
//{
//	Super::HandleMatchHasStarted();
//
//	ABHGameGameState* const MyGameState = Cast<ABHGameGameState>(GameState);
//	MyGameState->RemainingTime = RoundTime;
//
//	// notify players
//	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
//	{
//		ABHGamePlayerController* PC = Cast<ABHGamePlayerController>(*It);
//		if (PC)
//		{
//			PC->ClientGameStarted();
//		}
//	}
//}
//
//void ABHGameGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
//{
//	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
//}
//
//void ABHGameGameMode::FinishMatch()
//{
//	ABHGameGameState* const MyGameState = Cast<ABHGameGameState>(GameState);
//	if (IsMatchInProgress())
//	{
//		EndMatch();
//		DetermineMatchWinner();
//
//		// notify players
//		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
//		{
//			ABHGamePlayerState* PlayerState = Cast<ABHGamePlayerState>((*It)->PlayerState);
//			const bool bIsWinner = IsWinner(PlayerState);
//
//			(*It)->GameHasEnded(NULL, bIsWinner);
//		}
//
//		// lock all pawns
//		// pawns are not marked as keep for seamless travel, so we will create new pawns on the next match rather than
//		// turning these back on.
//		for (APawn* Pawn : TActorRange<APawn>(GetWorld()))
//		{
//			Pawn->TurnOff();
//		}
//
//		// set up to restart the match
//		MyGameState->RemainingTime = TimeBetweenMatches;
//	}
//}
//
//void ABHGameGameMode::RequestFinishAndExitToMainMenu()
//{
//	FinishMatch();
//
//	UBHGameGameInstance* const GameInstance = Cast<UBHGameGameInstance>(GetGameInstance());
//	if (GameInstance)
//	{
//		GameInstance->RemoveSplitScreenPlayers();
//	}
//
//	ABHGamePlayerController* LocalPrimaryController = nullptr;
//	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
//	{
//		ABHGamePlayerController* Controller = Cast<ABHGamePlayerController>(*Iterator);
//
//		if (Controller == NULL)
//		{
//			continue;
//		}
//
//		if (!Controller->IsLocalController())
//		{
//			const FText RemoteReturnReason = NSLOCTEXT("NetworkErrors", "HostHasLeft", "Host has left the game.");
//			Controller->ClientReturnToMainMenuWithTextReason(RemoteReturnReason);
//		}
//		else
//		{
//			LocalPrimaryController = Controller;
//		}
//	}
//
//	// GameInstance should be calling this from an EndState.  So call the PC function that performs cleanup, not the one that sets GI state.
//	if (LocalPrimaryController != NULL)
//	{
//		LocalPrimaryController->HandleReturnToMainMenu();
//	}
//}
//
//void ABHGameGameMode::DetermineMatchWinner()
//{
//	// nothing to do here
//}
//
//bool ABHGameGameMode::IsWinner(class ABHGamePlayerState* PlayerState) const
//{
//	return false;
//}
//
//void ABHGameGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
//{
//	ABHGameGameState* const MyGameState = Cast<ABHGameGameState>(GameState);
//	const bool bMatchIsOver = MyGameState && MyGameState->HasMatchEnded();
//	if (bMatchIsOver)
//	{
//		ErrorMessage = TEXT("Match is over!");
//	}
//	else
//	{
//		// GameSession can be NULL if the match is over
//		Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
//	}
//}
//
//
//void ABHGameGameMode::PostLogin(APlayerController* NewPlayer)
//{
//	Super::PostLogin(NewPlayer);
//
//	// update spectator location for client
//	ABHGamePlayerController* NewPC = Cast<ABHGamePlayerController>(NewPlayer);
//	if (NewPC && NewPC->GetPawn() == NULL)
//	{
//		NewPC->ClientSetSpectatorCamera(NewPC->GetSpawnLocation(), NewPC->GetControlRotation());
//	}
//
//	// notify new player if match is already in progress
//	if (NewPC && IsMatchInProgress())
//	{
//		NewPC->ClientGameStarted();
//		NewPC->ClientStartOnlineGame();
//	}
//}
//
//void ABHGameGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType)
//{
//	ABHGamePlayerState* KillerPlayerState = Killer ? Cast<ABHGamePlayerState>(Killer->PlayerState) : NULL;
//	ABHGamePlayerState* VictimPlayerState = KilledPlayer ? Cast<ABHGamePlayerState>(KilledPlayer->PlayerState) : NULL;
//
//	if (KillerPlayerState && KillerPlayerState != VictimPlayerState)
//	{
//		KillerPlayerState->ScoreKill(VictimPlayerState, KillScore);
//		KillerPlayerState->InformAboutKill(KillerPlayerState, DamageType, VictimPlayerState);
//	}
//
//	if (VictimPlayerState)
//	{
//		VictimPlayerState->ScoreDeath(KillerPlayerState, DeathScore);
//		VictimPlayerState->BroadcastDeath(KillerPlayerState, DamageType, VictimPlayerState);
//	}
//}
//
//float ABHGameGameMode::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
//{
//	float ActualDamage = Damage;
//
//	ABHGameCharacter* DamagedPawn = Cast<ABHGameCharacter>(DamagedActor);
//	if (DamagedPawn && EventInstigator)
//	{
//		ABHGamePlayerState* DamagedPlayerState = Cast<ABHGamePlayerState>(DamagedPawn->GetPlayerState());
//		ABHGamePlayerState* InstigatorPlayerState = Cast<ABHGamePlayerState>(EventInstigator->PlayerState);
//
//		// scale self instigated damage
//		if (InstigatorPlayerState == DamagedPlayerState)
//		{
//			ActualDamage *= DamageSelfScale;
//		}
//	}
//
//	return ActualDamage;
//}
//
//bool ABHGameGameMode::CanDealDamage(class ABHGamePlayerState* DamageInstigator, class ABHGamePlayerState* DamagedPlayer) const
//{
//	return true;
//}
//
//UClass* ABHGameGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
//{
//	if (InController->IsA<ABHGameAIController>())
//	{
//		return BotPawnClass;
//	}
//
//	return Super::GetDefaultPawnClassForController_Implementation(InController);
//}
//
//void ABHGameGameMode::RestartPlayer(AController* NewPlayer)
//{
//	Super::RestartPlayer(NewPlayer);
//
//	ABHGamePlayerController* PC = Cast<ABHGamePlayerController>(NewPlayer);
//	if (PC)
//	{
//		// Since initial weapon is equipped before the pawn is added to the replication graph, need to resend the notify so that it can be added as a dependent actor
//		ABHGameCharacter* Character = Cast<ABHGameCharacter>(PC->GetCharacter());
//		if (Character)
//		{
//			ABHGameCharacter::NotifyEquipWeapon.Broadcast(Character, Character->GetWeapon());
//		}
//
//		PC->ClientGameStarted();
//	}
//}
//
//AActor* ABHGameGameMode::ChoosePlayerStart_Implementation(AController* Player)
//{
//	TArray<APlayerStart*> PreferredSpawns;
//	TArray<APlayerStart*> FallbackSpawns;
//
//	APlayerStart* BestStart = NULL;
//	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
//	{
//		APlayerStart* TestSpawn = *It;
//		if (TestSpawn->IsA<APlayerStartPIE>())
//		{
//			// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
//			BestStart = TestSpawn;
//			break;
//		}
//		else
//		{
//			if (IsSpawnpointAllowed(TestSpawn, Player))
//			{
//				if (IsSpawnpointPreferred(TestSpawn, Player))
//				{
//					PreferredSpawns.Add(TestSpawn);
//				}
//				else
//				{
//					FallbackSpawns.Add(TestSpawn);
//				}
//			}
//		}
//	}
//
//
//	if (BestStart == NULL)
//	{
//		if (PreferredSpawns.Num() > 0)
//		{
//			BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
//		}
//		else if (FallbackSpawns.Num() > 0)
//		{
//			BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
//		}
//	}
//
//	return BestStart ? BestStart : Super::ChoosePlayerStart_Implementation(Player);
//}
//
//bool ABHGameGameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
//{
//	ABHGameTeamStart* BHGameSpawnPoint = Cast<ABHGameTeamStart>(SpawnPoint);
//	if (BHGameSpawnPoint)
//	{
//		ABHGameAIController* AIController = Cast<ABHGameAIController>(Player);
//		if (BHGameSpawnPoint->bNotForBots && AIController)
//		{
//			return false;
//		}
//
//		if (BHGameSpawnPoint->bNotForPlayers && AIController == NULL)
//		{
//			return false;
//		}
//		return true;
//	}
//
//	return false;
//}
//
//bool ABHGameGameMode::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const
//{
//	ACharacter* MyPawn = Cast<ACharacter>((*DefaultPawnClass)->GetDefaultObject<ACharacter>());
//	ABHGameAIController* AIController = Cast<ABHGameAIController>(Player);
//	if (AIController != nullptr)
//	{
//		MyPawn = Cast<ACharacter>(BotPawnClass->GetDefaultObject<ACharacter>());
//	}
//
//	if (MyPawn)
//	{
//		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
//		for (ACharacter* OtherPawn : TActorRange<ACharacter>(GetWorld()))
//		{
//			if (OtherPawn != MyPawn)
//			{
//				const float CombinedHeight = (MyPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * 2.0f;
//				const float CombinedRadius = MyPawn->GetCapsuleComponent()->GetScaledCapsuleRadius() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleRadius();
//				const FVector OtherLocation = OtherPawn->GetActorLocation();
//
//				// check if player start overlaps this pawn
//				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedRadius)
//				{
//					return false;
//				}
//			}
//		}
//	}
//	else
//	{
//		return false;
//	}
//
//	return true;
//}
//
//void ABHGameGameMode::RestartGame()
//{
//	// Hide the scoreboard too !
//	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
//	{
//		ABHGamePlayerController* PlayerController = Cast<ABHGamePlayerController>(*It);
//		if (PlayerController != nullptr)
//		{
//			ABHGameHUD* BHGameHUD = Cast<ABHGameHUD>(PlayerController->GetHUD());
//			if (BHGameHUD != nullptr)
//			{
//				// Passing true to bFocus here ensures that focus is returned to the game viewport.
//				BHGameHUD->ShowScoreboard(false, true);
//			}
//		}
//	}
//
//	Super::RestartGame();
//}

