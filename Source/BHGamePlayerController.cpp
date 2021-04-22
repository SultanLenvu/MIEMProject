#include "BHGamePlayerController.h"
#include "BHGamePlayerCameraManager.h"
//#include "Player/BHGameLocalPlayer.h"
#include "BHGamePlayerState.h"
#include "BHGameWeapon.h"
//#include "UI/Menu/BHGameIngameMenu.h"
//#include "UI/Style/BHGameStyle.h"
#include "BHGameHUD.h"
//#include "BHGameGameInstance.h"
//#include "BHGameLeaderboards.h"
//#include "BHGameGameViewportClient.h"
#include "SoundNodeLocalPlayer.h"
#include "AudioThread.h"
#include "Net/UnrealNetwork.h"
#include "BHGameCharacter.h"

ABHGamePlayerController::ABHGamePlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ABHGamePlayerCameraManager::StaticClass();
	bAllowGameActions = true;
	bGameEndedFrame = false;
	LastDeathLocation = FVector::ZeroVector;

	//ServerSayString = TEXT("Say");
	//BHGameFriendUpdateTimer = 0.0f;
	//bHasSentStartEvents = false;

	//StatMatchesPlayed = 0;
	//StatKills = 0;
	//StatDeaths = 0;
	//bHasQueriedPlatformStats = false;
	//bHasQueriedPlatformAchievements = false;
	bHasInitializedInputComponent = false;
}

void ABHGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (!bHasInitializedInputComponent)
	{
		// UI input
		//InputComponent->BindAction("InGameMenu", IE_Pressed, this, &ABHGamePlayerController::OnToggleInGameMenu);
		//InputComponent->BindAction("Scoreboard", IE_Pressed, this, &ABHGamePlayerController::OnShowScoreboard);
		//InputComponent->BindAction("Scoreboard", IE_Released, this, &ABHGamePlayerController::OnHideScoreboard);
		//InputComponent->BindAction("ConditionalCloseScoreboard", IE_Pressed, this, &ABHGamePlayerController::OnConditionalCloseScoreboard);
		//InputComponent->BindAction("ToggleScoreboard", IE_Pressed, this, &ABHGamePlayerController::OnToggleScoreboard);

		// voice chat
		//InputComponent->BindAction("PushToTalk", IE_Pressed, this, &APlayerController::StartTalking);
		//InputComponent->BindAction("PushToTalk", IE_Released, this, &APlayerController::StopTalking);

		//InputComponent->BindAction("ToggleChat", IE_Pressed, this, &ABHGamePlayerController::ToggleChatWindow);

		bHasInitializedInputComponent = true;
	}
}


void ABHGamePlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//FBHGameStyle::Initialize();
	//BHGameFriendUpdateTimer = 0;
}

//void ABHGamePlayerController::ClearLeaderboardDelegate()
//{
//	IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld());
//	if (OnlineSub)
//	{
//		IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
//		if (Leaderboards.IsValid())
//		{
//			Leaderboards->ClearOnLeaderboardReadCompleteDelegate_Handle(LeaderboardReadCompleteDelegateHandle);
//		}
//	}
//}

void ABHGamePlayerController::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	//if (IsGameMenuVisible())
	//{
	//	if (BHGameFriendUpdateTimer > 0)
	//	{
	//		BHGameFriendUpdateTimer -= DeltaTime;
	//	}
	//	else
	//	{
	//		TSharedPtr<class FBHGameFriends> BHGameFriends = BHGameIngameMenu->GetBHGameFriends();
	//		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	//		if (BHGameFriends.IsValid() && LocalPlayer && LocalPlayer->GetControllerId() >= 0)
	//		{
	//			BHGameFriends->UpdateFriends(LocalPlayer->GetControllerId());
	//		}

	//		// Make sure the time between calls is long enough that we won't trigger (0x80552C81) and not exceed the web api rate limit
	//		// That value is currently 75 requests / 15 minutes.
	//		BHGameFriendUpdateTimer = 15;

	//	}
	//}

	//// Is this the first frame after the game has ended
	//if (bGameEndedFrame)
	//{
	//	bGameEndedFrame = false;

	//	// ONLY PUT CODE HERE WHICH YOU DON'T WANT TO BE DONE DUE TO HOST LOSS

	//	// Do we need to show the end of round scoreboard?
	//	if (IsPrimaryPlayer())
	//	{
	//		ABHGameHUD* BHGameHUD = GetBHGameHUD();
	//		if (BHGameHUD)
	//		{
	//			BHGameHUD->ShowScoreboard(true, true);
	//		}
	//	}
	//}

	const bool bLocallyControlled = IsLocalController();
	const uint32 UniqueID = GetUniqueID();
	FAudioThread::RunCommandOnAudioThread([UniqueID, bLocallyControlled]()
	{
		USoundNodeLocalPlayer::GetLocallyControlledActorCache().Add(UniqueID, bLocallyControlled);
	});
};

void ABHGamePlayerController::BeginDestroy()
{
	Super::BeginDestroy();
	//ClearLeaderboardDelegate();

	// clear any online subsystem references
	BHGameIngameMenu = nullptr;

	if (!GExitPurge)
	{
		const uint32 UniqueID = GetUniqueID();
		FAudioThread::RunCommandOnAudioThread([UniqueID]()
		{
			USoundNodeLocalPlayer::GetLocallyControlledActorCache().Remove(UniqueID);
		});
	}
}

void ABHGamePlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	//if (ULocalPlayer* const LocalPlayer = Cast<ULocalPlayer>(Player))
	//{
	//	//Build menu only after game is initialized
	//	BHGameIngameMenu = MakeShareable(new FBHGameIngameMenu());
	//	BHGameIngameMenu->Construct(Cast<ULocalPlayer>(Player));

	//	FInputModeGameOnly InputMode;
	//	SetInputMode(InputMode);
	//}
}

//void ABHGamePlayerController::OnLeaderboardReadComplete(bool bWasSuccessful)
//{
//	if (ReadObject.IsValid() && ReadObject->ReadState == EOnlineAsyncTaskState::Done && !bHasQueriedPlatformStats)
//	{
//		bHasQueriedPlatformStats = true;
//		ClearLeaderboardDelegate();
//
//		// We should only have one stat.
//		if (bWasSuccessful && ReadObject->Rows.Num() == 1)
//		{
//			FOnlineStatsRow& RowData = ReadObject->Rows[0];
//			if (const FVariantData* KillData = RowData.Columns.Find(LEADERBOARD_STAT_KILLS))
//			{
//				KillData->GetValue(StatKills);
//			}
//
//			if (const FVariantData* DeathData = RowData.Columns.Find(LEADERBOARD_STAT_DEATHS))
//			{
//				DeathData->GetValue(StatDeaths);
//			}
//
//			if (const FVariantData* MatchData = RowData.Columns.Find(LEADERBOARD_STAT_MATCHESPLAYED))
//			{
//				MatchData->GetValue(StatMatchesPlayed);
//			}
//
//			UE_LOG(LogOnline, Log, TEXT("Fetched player stat data. Kills %d Deaths %d Matches %d"), StatKills, StatDeaths, StatMatchesPlayed);
//		}
//	}
//}

//void ABHGamePlayerController::QueryStats()
//{
//	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
//	if (LocalPlayer && LocalPlayer->GetControllerId() != -1)
//	{
//		IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld());
//		if (OnlineSub)
//		{
//			IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
//			if (Identity.IsValid())
//			{
//				TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->GetControllerId());
//
//				if (UserId.IsValid())
//				{
//					IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
//					if (Leaderboards.IsValid() && !bHasQueriedPlatformStats)
//					{
//						TArray<TSharedRef<const FUniqueNetId>> QueryPlayers;
//						QueryPlayers.Add(UserId.ToSharedRef());
//
//						LeaderboardReadCompleteDelegateHandle = Leaderboards->OnLeaderboardReadCompleteDelegates.AddUObject(this, &ABHGamePlayerController::OnLeaderboardReadComplete);
//						ReadObject = MakeShareable(new FBHGameAllTimeMatchResultsRead());
//						FOnlineLeaderboardReadRef ReadObjectRef = ReadObject.ToSharedRef();
//						if (Leaderboards->ReadLeaderboards(QueryPlayers, ReadObjectRef))
//						{
//							UE_LOG(LogOnline, Log, TEXT("Started process to fetch stats for current user."));
//						}
//						else
//						{
//							UE_LOG(LogOnline, Warning, TEXT("Could not start leaderboard fetch process. This will affect stat writes for this session."));
//						}
//
//					}
//				}
//			}
//		}
//	}
//}

void ABHGamePlayerController::UnFreeze()
{
	ServerRestartPlayer();
}

void ABHGamePlayerController::FailedToSpawnPawn()
{
	if (StateName == NAME_Inactive)
	{
		BeginInactiveState();
	}
	Super::FailedToSpawnPawn();
}

void ABHGamePlayerController::PawnPendingDestroy(APawn* P)
{
	LastDeathLocation = P->GetActorLocation();
	FVector CameraLocation = LastDeathLocation + FVector(0, 0, 300.0f);
	FRotator CameraRotation(-90.0f, 0.0f, 0.0f);
	FindDeathCameraSpot(CameraLocation, CameraRotation);

	Super::PawnPendingDestroy(P);

	//ClientSetSpectatorCamera(CameraLocation, CameraRotation);
}

//void ABHGamePlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
//{
//	Super::GameHasEnded(EndGameFocus, bIsWinner);
//}

//void ABHGamePlayerController::ClientSetSpectatorCamera_Implementation(FVector CameraLocation, FRotator CameraRotation)
//{
//	SetInitialLocationAndRotation(CameraLocation, CameraRotation);
//	SetViewTarget(this);
//}

bool ABHGamePlayerController::FindDeathCameraSpot(FVector& CameraLocation, FRotator& CameraRotation)
{
	const FVector PawnLocation = GetPawn()->GetActorLocation();
	FRotator ViewDir = GetControlRotation();
	ViewDir.Pitch = -45.0f;

	const float YawOffsets[] = { 0.0f, -180.0f, 90.0f, -90.0f, 45.0f, -45.0f, 135.0f, -135.0f };
	const float CameraOffset = 600.0f;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(DeathCamera), true, GetPawn());

	FHitResult HitResult;
	for (int32 i = 0; i < UE_ARRAY_COUNT(YawOffsets); i++)
	{
		FRotator CameraDir = ViewDir;
		CameraDir.Yaw += YawOffsets[i];
		CameraDir.Normalize();

		const FVector TestLocation = PawnLocation - CameraDir.Vector() * CameraOffset;

		const bool bBlocked = GetWorld()->LineTraceSingleByChannel(HitResult, PawnLocation, TestLocation, ECC_Camera, TraceParams);

		if (!bBlocked)
		{
			CameraLocation = TestLocation;
			CameraRotation = CameraDir;
			return true;
		}
	}

	return false;
}

//void ABHGamePlayerController::OnKill()
//{
//	const UWorld* World = GetWorld();
//
//	//const IOnlineEventsPtr Events = Online::GetEventsInterface(World);
//	//const IOnlineIdentityPtr Identity = Online::GetIdentityInterface(World);
//
//	if (Events.IsValid() && Identity.IsValid())
//	{
//		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
//		if (LocalPlayer)
//		{
//			int32 UserIndex = LocalPlayer->GetControllerId();
//			TSharedPtr<const FUniqueNetId> UniqueID = Identity->GetUniquePlayerId(UserIndex);
//			if (UniqueID.IsValid())
//			{
//				ABHGameCharacter* BHGameChar = Cast<ABHGameCharacter>(GetCharacter());
//				// If player is dead, use location stored during pawn cleanup.
//				FVector Location = BHGameChar ? BHGameChar->GetActorLocation() : LastDeathLocation;
//				ABHGameWeapon* Weapon = BHGameChar ? BHGameChar->GetWeapon() : 0;
//				int32 WeaponType = Weapon ? (int32)Weapon->GetAmmoType() : 0;
//
//				FOnlineEventParms Params;
//
//				Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // @todo determine game mode (ffa v tdm)
//				Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused
//
//				Params.Add(TEXT("PlayerRoleId"), FVariantData((int32)0)); // unused
//				Params.Add(TEXT("PlayerWeaponId"), FVariantData((int32)WeaponType));
//				Params.Add(TEXT("EnemyRoleId"), FVariantData((int32)0)); // unused
//				Params.Add(TEXT("EnemyWeaponId"), FVariantData((int32)0)); // untracked			
//				Params.Add(TEXT("KillTypeId"), FVariantData((int32)0)); // unused
//				Params.Add(TEXT("LocationX"), FVariantData(Location.X));
//				Params.Add(TEXT("LocationY"), FVariantData(Location.Y));
//				Params.Add(TEXT("LocationZ"), FVariantData(Location.Z));
//
//				Events->TriggerEvent(*UniqueID, TEXT("KillOponent"), Params);
//			}
//		}
//	}
//}

void ABHGamePlayerController::OnDeathMessage(class ABHGamePlayerState* KillerPlayerState, class ABHGamePlayerState* KilledPlayerState, const UDamageType* KillerDamageType)
{
	//ABHGameHUD* BHGameHUD = GetBHGameHUD();
	//if (BHGameHUD)
	//{
	//	BHGameHUD->ShowDeathMessage(KillerPlayerState, KilledPlayerState, KillerDamageType);
	//}

	//ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	//if (LocalPlayer && LocalPlayer->GetCachedUniqueNetId().IsValid() && KilledPlayerState->GetUniqueId().IsValid())
	//{
	//	// if this controller is the player who died, update the hero stat.
	//	if (*LocalPlayer->GetCachedUniqueNetId() == *KilledPlayerState->GetUniqueId())
	//	{
	//		const UWorld* World = GetWorld();
	//		const IOnlineEventsPtr Events = Online::GetEventsInterface(World);
	//		const IOnlineIdentityPtr Identity = Online::GetIdentityInterface(World);

	//		if (Events.IsValid() && Identity.IsValid())
	//		{
	//			const int32 UserIndex = LocalPlayer->GetControllerId();
	//			TSharedPtr<const FUniqueNetId> UniqueID = Identity->GetUniquePlayerId(UserIndex);
	//			if (UniqueID.IsValid())
	//			{
	//				ABHGameCharacter* BHGameChar = Cast<ABHGameCharacter>(GetCharacter());
	//				ABHGameWeapon* Weapon = BHGameChar ? BHGameChar->GetWeapon() : NULL;

	//				FVector Location = BHGameChar ? BHGameChar->GetActorLocation() : FVector::ZeroVector;
	//				int32 WeaponType = Weapon ? (int32)Weapon->GetAmmoType() : 0;

	//				FOnlineEventParms Params;
	//				Params.Add(TEXT("SectionId"), FVariantData((int32)0)); // unused
	//				Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // @todo determine game mode (ffa v tdm)
	//				Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused

	//				Params.Add(TEXT("PlayerRoleId"), FVariantData((int32)0)); // unused
	//				Params.Add(TEXT("PlayerWeaponId"), FVariantData((int32)WeaponType));
	//				Params.Add(TEXT("EnemyRoleId"), FVariantData((int32)0)); // unused
	//				Params.Add(TEXT("EnemyWeaponId"), FVariantData((int32)0)); // untracked

	//				Params.Add(TEXT("LocationX"), FVariantData(Location.X));
	//				Params.Add(TEXT("LocationY"), FVariantData(Location.Y));
	//				Params.Add(TEXT("LocationZ"), FVariantData(Location.Z));

	//				Events->TriggerEvent(*UniqueID, TEXT("PlayerDeath"), Params);
	//			}
	//		}
	//	}
	//}
}

//void ABHGamePlayerController::OnToggleInGameMenu()
//{
//	if (GEngine->GameViewport == nullptr)
//	{
//		return;
//	}
//
//	// this is not ideal, but necessary to prevent both players from pausing at the same time on the same frame
//	UWorld* GameWorld = GEngine->GameViewport->GetWorld();
//
//	for (auto It = GameWorld->GetControllerIterator(); It; ++It)
//	{
//		ABHGamePlayerController* Controller = Cast<ABHGamePlayerController>(*It);
//		if (Controller && Controller->IsPaused())
//		{
//			return;
//		}
//	}
//
//	// if no one's paused, pause
//	if (BHGameIngameMenu.IsValid())
//	{
//		BHGameIngameMenu->ToggleGameMenu();
//	}
//}

//void ABHGamePlayerController::OnConditionalCloseScoreboard()
//{
//	ABHGameHUD* BHGameHUD = GetBHGameHUD();
//	if (BHGameHUD && (BHGameHUD->IsMatchOver() == false))
//	{
//		BHGameHUD->ConditionalCloseScoreboard();
//	}
//}

//void ABHGamePlayerController::OnToggleScoreboard()
//{
//	ABHGameHUD* BHGameHUD = GetBHGameHUD();
//	if (BHGameHUD && (BHGameHUD->IsMatchOver() == false))
//	{
//		BHGameHUD->ToggleScoreboard();
//	}
//}

//void ABHGamePlayerController::OnShowScoreboard()
//{
//	ABHGameHUD* BHGameHUD = GetBHGameHUD();
//	if (BHGameHUD)
//	{
//		BHGameHUD->ShowScoreboard(true);
//	}
//}

//void ABHGamePlayerController::OnHideScoreboard()
//{
//	ABHGameHUD* BHGameHUD = GetBHGameHUD();
//	// If have a valid match and the match is over - hide the scoreboard
//	if ((BHGameHUD != NULL) && (BHGameHUD->IsMatchOver() == false))
//	{
//		BHGameHUD->ShowScoreboard(false);
//	}
//}

//bool ABHGamePlayerController::IsGameMenuVisible() const
//{
//	bool Result = false;
//	if (BHGameIngameMenu.IsValid())
//	{
//		Result = BHGameIngameMenu->GetIsGameMenuUp();
//	}
//
//	return Result;
//}

void ABHGamePlayerController::SetHealthRegen(bool bEnable)
{
	bHealthRegen = bEnable;
}

//void ABHGamePlayerController::ClientGameStarted_Implementation()
//{
//	bAllowGameActions = true;
//
//	// Enable controls mode now the game has started
//	SetIgnoreMoveInput(false);
//
//	ABHGameHUD* BHGameHUD = GetBHGameHUD();
//	if (BHGameHUD)
//	{
//		BHGameHUD->SetMatchState(EBHGameMatchState::Playing);
//		BHGameHUD->ShowScoreboard(false);
//	}
//	bGameEndedFrame = false;
//
//	QueryStats();
//
//	const UWorld* World = GetWorld();
//
//	// Send round start event
//	const IOnlineEventsPtr Events = Online::GetEventsInterface(World);
//	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
//
//	if (LocalPlayer != nullptr && World != nullptr && Events.IsValid())
//	{
//		FUniqueNetIdRepl UniqueId = LocalPlayer->GetPreferredUniqueNetId();
//
//		if (UniqueId.IsValid())
//		{
//			// Generate a new session id
//			Events->SetPlayerSessionId(*UniqueId, FGuid::NewGuid());
//
//			FString MapName = *FPackageName::GetShortName(World->PersistentLevel->GetOutermost()->GetName());
//
//			// Fire session start event for all cases
//			FOnlineEventParms Params;
//			Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // @todo determine game mode (ffa v tdm)
//			Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused
//			Params.Add(TEXT("MapName"), FVariantData(MapName));
//
//			Events->TriggerEvent(*UniqueId, TEXT("PlayerSessionStart"), Params);
//
//			// Online matches require the MultiplayerRoundStart event as well
//			UBHGameGameInstance* SGI = Cast<UBHGameGameInstance>(World->GetGameInstance());
//
//			if (SGI && (SGI->GetOnlineMode() == EOnlineMode::Online))
//			{
//				FOnlineEventParms MultiplayerParams;
//
//				// @todo: fill in with real values
//				MultiplayerParams.Add(TEXT("SectionId"), FVariantData((int32)0)); // unused
//				MultiplayerParams.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // @todo determine game mode (ffa v tdm)
//				MultiplayerParams.Add(TEXT("MatchTypeId"), FVariantData((int32)1)); // @todo abstract the specific meaning of this value across platforms
//				MultiplayerParams.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused
//
//				Events->TriggerEvent(*UniqueId, TEXT("MultiplayerRoundStart"), MultiplayerParams);
//			}
//
//			bHasSentStartEvents = true;
//		}
//	}
//}

/** Starts the online game using the session name in the PlayerState */
//void ABHGamePlayerController::ClientStartOnlineGame_Implementation()
//{
//	if (!IsPrimaryPlayer())
//		return;
//
//	ABHGamePlayerState* BHGamePlayerState = Cast<ABHGamePlayerState>(PlayerState);
//	if (BHGamePlayerState)
//	{
//		IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld());
//		if (OnlineSub)
//		{
//			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
//			if (Sessions.IsValid() && (Sessions->GetNamedSession(BHGamePlayerState->SessionName) != nullptr))
//			{
//				UE_LOG(LogOnline, Log, TEXT("Starting session %s on client"), *BHGamePlayerState->SessionName.ToString());
//				Sessions->StartSession(BHGamePlayerState->SessionName);
//			}
//		}
//	}
//	else
//	{
//		// Keep retrying until player state is replicated
//		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ClientStartOnlineGame, this, &ABHGamePlayerController::ClientStartOnlineGame_Implementation, 0.2f, false);
//	}
//}

/** Ends the online game using the session name in the PlayerState */
//void ABHGamePlayerController::ClientEndOnlineGame_Implementation()
//{
//	if (!IsPrimaryPlayer())
//		return;
//
//	ABHGamePlayerState* BHGamePlayerState = Cast<ABHGamePlayerState>(PlayerState);
//	if (BHGamePlayerState)
//	{
//		IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld());
//		if (OnlineSub)
//		{
//			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
//			if (Sessions.IsValid() && (Sessions->GetNamedSession(BHGamePlayerState->SessionName) != nullptr))
//			{
//				UE_LOG(LogOnline, Log, TEXT("Ending session %s on client"), *BHGamePlayerState->SessionName.ToString());
//				Sessions->EndSession(BHGamePlayerState->SessionName);
//			}
//		}
//	}
//}

//void ABHGamePlayerController::HandleReturnToMainMenu()
//{
//	OnHideScoreboard();
//	CleanupSessionOnReturnToMenu();
//}

//void ABHGamePlayerController::ClientReturnToMainMenu_Implementation(const FString& InReturnReason)
//{
//	const UWorld* World = GetWorld();
//	UBHGameGameInstance* SGI = World != NULL ? Cast<UBHGameGameInstance>(World->GetGameInstance()) : NULL;
//
//	if (!ensure(SGI != NULL))
//	{
//		return;
//	}
//
//	if (GetNetMode() == NM_Client)
//	{
//		const FText ReturnReason = NSLOCTEXT("NetworkErrors", "HostQuit", "The host has quit the match.");
//		const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
//
//		SGI->ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), BHGameGameInstanceState::MainMenu);
//	}
//	else
//	{
//		SGI->GotoState(BHGameGameInstanceState::MainMenu);
//	}
//
//	// Clear the flag so we don't do normal end of round stuff next
//	bGameEndedFrame = false;
//}

/** Ends and/or destroys game session */
//void ABHGamePlayerController::CleanupSessionOnReturnToMenu()
//{
//	const UWorld* World = GetWorld();
//	UBHGameGameInstance* SGI = World != NULL ? Cast<UBHGameGameInstance>(World->GetGameInstance()) : NULL;
//
//	if (ensure(SGI != NULL))
//	{
//		SGI->CleanupSessionOnReturnToMenu();
//	}
//}

//void ABHGamePlayerController::ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner)
//{
//	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);
//
//	// Disable controls now the game has ended
//	SetIgnoreMoveInput(true);
//
//	bAllowGameActions = false;
//
//	// Make sure that we still have valid view target
//	SetViewTarget(GetPawn());
//
//	ABHGameHUD* BHGameHUD = GetBHGameHUD();
//	if (BHGameHUD)
//	{
//		BHGameHUD->SetMatchState(bIsWinner ? EBHGameMatchState::Won : EBHGameMatchState::Lost);
//	}
//
//	UpdateSaveFileOnGameEnd(bIsWinner);
//	UpdateAchievementsOnGameEnd();
//	UpdateLeaderboardsOnGameEnd();
//	UpdateStatsOnGameEnd(bIsWinner);
//
//	// Flag that the game has just ended (if it's ended due to host loss we want to wait for ClientReturnToMainMenu_Implementation first, incase we don't want to process)
//	bGameEndedFrame = true;
//}

//void ABHGamePlayerController::ClientSendRoundEndEvent_Implementation(bool bIsWinner, int32 ExpendedTimeInSeconds)
//{
//	const UWorld* World = GetWorld();
//	const IOnlineEventsPtr Events = Online::GetEventsInterface(World);
//	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
//
//	if (bHasSentStartEvents && LocalPlayer != nullptr && World != nullptr && Events.IsValid())
//	{
//		FUniqueNetIdRepl UniqueId = LocalPlayer->GetPreferredUniqueNetId();
//
//		if (UniqueId.IsValid())
//		{
//			FString MapName = *FPackageName::GetShortName(World->PersistentLevel->GetOutermost()->GetName());
//			ABHGamePlayerState* BHGamePlayerState = Cast<ABHGamePlayerState>(PlayerState);
//			int32 PlayerScore = BHGamePlayerState ? BHGamePlayerState->GetScore() : 0;
//			int32 PlayerDeaths = BHGamePlayerState ? BHGamePlayerState->GetDeaths() : 0;
//			int32 PlayerKills = BHGamePlayerState ? BHGamePlayerState->GetKills() : 0;
//
//			// Fire session end event for all cases
//			FOnlineEventParms Params;
//			Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // @todo determine game mode (ffa v tdm)
//			Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused
//			Params.Add(TEXT("ExitStatusId"), FVariantData((int32)0)); // unused
//			Params.Add(TEXT("PlayerScore"), FVariantData((int32)PlayerScore));
//			Params.Add(TEXT("PlayerWon"), FVariantData((bool)bIsWinner));
//			Params.Add(TEXT("MapName"), FVariantData(MapName));
//			Params.Add(TEXT("MapNameString"), FVariantData(MapName)); // @todo workaround for a bug in backend service, remove when fixed
//
//			Events->TriggerEvent(*UniqueId, TEXT("PlayerSessionEnd"), Params);
//
//			// Update all time results
//			FOnlineEventParms AllTimeMatchParams;
//			AllTimeMatchParams.Add(TEXT("BHGameAllTimeMatchResultsScore"), FVariantData((uint64)PlayerScore));
//			AllTimeMatchParams.Add(TEXT("BHGameAllTimeMatchResultsDeaths"), FVariantData((int32)PlayerDeaths));
//			AllTimeMatchParams.Add(TEXT("BHGameAllTimeMatchResultsFrags"), FVariantData((int32)PlayerKills));
//			AllTimeMatchParams.Add(TEXT("BHGameAllTimeMatchResultsMatchesPlayed"), FVariantData((int32)1));
//
//			Events->TriggerEvent(*UniqueId, TEXT("BHGameAllTimeMatchResults"), AllTimeMatchParams);
//
//			// Online matches require the MultiplayerRoundEnd event as well
//			UBHGameGameInstance* SGI = Cast<UBHGameGameInstance>(World->GetGameInstance());
//			if (SGI && (SGI->GetOnlineMode() == EOnlineMode::Online))
//			{
//				FOnlineEventParms MultiplayerParams;
//				MultiplayerParams.Add(TEXT("SectionId"), FVariantData((int32)0)); // unused
//				MultiplayerParams.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // @todo determine game mode (ffa v tdm)
//				MultiplayerParams.Add(TEXT("MatchTypeId"), FVariantData((int32)1)); // @todo abstract the specific meaning of this value across platforms
//				MultiplayerParams.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused
//				MultiplayerParams.Add(TEXT("TimeInSeconds"), FVariantData((float)ExpendedTimeInSeconds));
//				MultiplayerParams.Add(TEXT("ExitStatusId"), FVariantData((int32)0)); // unused
//
//				Events->TriggerEvent(*UniqueId, TEXT("MultiplayerRoundEnd"), MultiplayerParams);
//			}
//		}
//
//		bHasSentStartEvents = false;
//	}
//}

//void ABHGamePlayerController::SetCinematicMode(bool bInCinematicMode, bool bHidePlayer, bool bAffectsHUD, bool bAffectsMovement, bool bAffectsTurning)
//{
//	Super::SetCinematicMode(bInCinematicMode, bHidePlayer, bAffectsHUD, bAffectsMovement, bAffectsTurning);
//
//	// If we have a pawn we need to determine if we should show/hide the weapon
//	ABHGameCharacter* MyPawn = Cast<ABHGameCharacter>(GetPawn());
//	ABHGameWeapon* MyWeapon = MyPawn ? MyPawn->GetWeapon() : NULL;
//	if (MyWeapon)
//	{
//		if (bInCinematicMode && bHidePlayer)
//		{
//			MyWeapon->SetActorHiddenInGame(true);
//		}
//		else if (!bCinematicMode)
//		{
//			MyWeapon->SetActorHiddenInGame(false);
//		}
//	}
//}

bool ABHGamePlayerController::IsMoveInputIgnored() const
{
	if (IsInState(NAME_Spectating))
	{
		return false;
	}
	else
	{
		return Super::IsMoveInputIgnored();
	}
}

bool ABHGamePlayerController::IsLookInputIgnored() const
{
	if (IsInState(NAME_Spectating))
	{
		return false;
	}
	else
	{
		return Super::IsLookInputIgnored();
	}
}

//void ABHGamePlayerController::InitInputSystem()
//{
//	Super::InitInputSystem();
//
//	UBHGamePersistentUser* PersistentUser = GetPersistentUser();
//	if (PersistentUser)
//	{
//		PersistentUser->TellInputAboutKeybindings();
//	}
//}

void ABHGamePlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABHGamePlayerController, bHealthRegen);
}

void ABHGamePlayerController::Suicide()
{
	if (IsInState(NAME_Playing))
	{
		ServerSuicide();
	}
}

bool ABHGamePlayerController::ServerSuicide_Validate()
{
	return true;
}

void ABHGamePlayerController::ServerSuicide_Implementation()
{
	if ((GetPawn() != NULL) && ((GetWorld()->TimeSeconds - GetPawn()->CreationTime > 1) || (GetNetMode() == NM_Standalone)))
	{
		ABHGameCharacter* MyPawn = Cast<ABHGameCharacter>(GetPawn());
		if (MyPawn)
		{
			MyPawn->Suicide();
		}
	}
}

bool ABHGamePlayerController::HasHealthRegen() const
{
	return bHealthRegen;
}

bool ABHGamePlayerController::IsGameInputAllowed() const
{
	return bAllowGameActions && !bCinematicMode;
}

//void ABHGamePlayerController::ToggleChatWindow()
//{
//	ABHGameHUD* BHGameHUD = Cast<ABHGameHUD>(GetHUD());
//	if (BHGameHUD)
//	{
//		BHGameHUD->ToggleChat();
//	}
//}

//void ABHGamePlayerController::ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime)
//{
//	ABHGameHUD* BHGameHUD = Cast<ABHGameHUD>(GetHUD());
//	if (BHGameHUD)
//	{
//		if (Type == ServerSayString)
//		{
//			if (SenderPlayerState != PlayerState)
//			{
//				BHGameHUD->AddChatLine(FText::FromString(S), false);
//			}
//		}
//	}
//}

//void ABHGamePlayerController::Say(const FString& Msg)
//{
//	ServerSay(Msg.Left(128));
//}
//
//bool ABHGamePlayerController::ServerSay_Validate(const FString& Msg)
//{
//	return true;
//}

//void ABHGamePlayerController::ServerSay_Implementation(const FString& Msg)
//{
//	GetWorld()->GetAuthGameMode<ABHGameGameMode>()->Broadcast(this, Msg, ServerSayString);
//}

ABHGameHUD* ABHGamePlayerController::GetBHGameHUD() const
{
	return Cast<ABHGameHUD>(GetHUD());
}


//UBHGamePersistentUser* ABHGamePlayerController::GetPersistentUser() const
//{
//	UBHGameLocalPlayer* const BHGameLocalPlayer = Cast<UBHGameLocalPlayer>(Player);
//	return BHGameLocalPlayer ? BHGameLocalPlayer->GetPersistentUser() : nullptr;
//}

//bool ABHGamePlayerController::SetPause(bool bPause, FCanUnpause CanUnpauseDelegate /*= FCanUnpause()*/)
//{
//	const bool Result = APlayerController::SetPause(bPause, CanUnpauseDelegate);
//
//	// Update rich presence.
//	const UWorld* World = GetWorld();
//	const IOnlinePresencePtr PresenceInterface = Online::GetPresenceInterface(World);
//	const IOnlineEventsPtr Events = Online::GetEventsInterface(World);
//	const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
//	FUniqueNetIdRepl UserId = LocalPlayer ? LocalPlayer->GetCachedUniqueNetId() : FUniqueNetIdRepl();
//
//	// Don't send pause events while online since the game doesn't actually pause
//	if (GetNetMode() == NM_Standalone && Events.IsValid() && PlayerState->GetUniqueId().IsValid())
//	{
//		FOnlineEventParms Params;
//		Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // @todo determine game mode (ffa v tdm)
//		Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused
//		if (Result && bPause)
//		{
//			Events->TriggerEvent(*PlayerState->GetUniqueId(), TEXT("PlayerSessionPause"), Params);
//		}
//		else
//		{
//			Events->TriggerEvent(*PlayerState->GetUniqueId(), TEXT("PlayerSessionResume"), Params);
//		}
//	}
//
//	return Result;
//}

//FVector ABHGamePlayerController::GetFocalLocation() const
//{
//	const ABHGameCharacter* BHGameCharacter = Cast<ABHGameCharacter>(GetPawn());
//
//	// On death we want to use the player's death cam location rather than the location of where the pawn is at the moment
//	// This guarantees that the clients see their death cam correctly, as their pawns have delayed destruction.
//	if (BHGameCharacter && BHGameCharacter->bIsDying)
//	{
//		return GetSpawnLocation();
//	}
//
//	return Super::GetFocalLocation();
//}

//void ABHGamePlayerController::ShowInGameMenu()
//{
//	ABHGameHUD* BHGameHUD = GetBHGameHUD();
//	if (BHGameIngameMenu.IsValid() && !BHGameIngameMenu->GetIsGameMenuUp() && BHGameHUD && (BHGameHUD->IsMatchOver() == false))
//	{
//		BHGameIngameMenu->ToggleGameMenu();
//	}
//}

//void ABHGamePlayerController::UpdateLeaderboardsOnGameEnd()
//{
//	UBHGameLocalPlayer* LocalPlayer = Cast<UBHGameLocalPlayer>(Player);
//	if (LocalPlayer)
//	{
//		// update leaderboards - note this does not respect existing scores and overwrites them. We would first need to read the leaderboards if we wanted to do that.
//		IOnlineSubsystem* const OnlineSub = Online::GetSubsystem(GetWorld());
//		if (OnlineSub)
//		{
//			IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
//			if (Identity.IsValid())
//			{
//				TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->GetControllerId());
//				if (UserId.IsValid())
//				{
//					IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
//					if (Leaderboards.IsValid())
//					{
//						ABHGamePlayerState* BHGamePlayerState = Cast<ABHGamePlayerState>(PlayerState);
//						if (BHGamePlayerState)
//						{
//							FBHGameAllTimeMatchResultsWrite ResultsWriteObject;
//							int32 MatchWriteData = 1;
//							int32 KillsWriteData = BHGamePlayerState->GetKills();
//							int32 DeathsWriteData = BHGamePlayerState->GetDeaths();
//
//#if TRACK_STATS_LOCALLY
//							StatMatchesPlayed = (MatchWriteData += StatMatchesPlayed);
//							StatKills = (KillsWriteData += StatKills);
//							StatDeaths = (DeathsWriteData += StatDeaths);
//#endif
//
//							ResultsWriteObject.SetIntStat(LEADERBOARD_STAT_SCORE, KillsWriteData);
//							ResultsWriteObject.SetIntStat(LEADERBOARD_STAT_KILLS, KillsWriteData);
//							ResultsWriteObject.SetIntStat(LEADERBOARD_STAT_DEATHS, DeathsWriteData);
//							ResultsWriteObject.SetIntStat(LEADERBOARD_STAT_MATCHESPLAYED, MatchWriteData);
//
//							// the call will copy the user id and write object to its own memory
//							Leaderboards->WriteLeaderboards(BHGamePlayerState->SessionName, *UserId, ResultsWriteObject);
//							Leaderboards->FlushLeaderboards(TEXT("BHGameGAME"));
//						}
//					}
//				}
//			}
//		}
//	}
//}

//void ABHGamePlayerController::UpdateStatsOnGameEnd(bool bIsWinner)
//{
//	const IOnlineStatsPtr Stats = Online::GetStatsInterface(GetWorld());
//	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
//	ABHGamePlayerState* BHGamePlayerState = Cast<ABHGamePlayerState>(PlayerState);
//
//	if (Stats.IsValid() && LocalPlayer != nullptr && BHGamePlayerState != nullptr)
//	{
//		FUniqueNetIdRepl UniqueId = LocalPlayer->GetCachedUniqueNetId();
//
//		if (UniqueId.IsValid())
//		{
//			TArray<FOnlineStatsUserUpdatedStats> UpdatedUserStats;
//
//			FOnlineStatsUserUpdatedStats& UpdatedStats = UpdatedUserStats.Emplace_GetRef(UniqueId.GetUniqueNetId().ToSharedRef());
//			UpdatedStats.Stats.Add(TEXT("Kills"), FOnlineStatUpdate(BHGamePlayerState->GetKills(), FOnlineStatUpdate::EOnlineStatModificationType::Sum));
//			UpdatedStats.Stats.Add(TEXT("Deaths"), FOnlineStatUpdate(BHGamePlayerState->GetDeaths(), FOnlineStatUpdate::EOnlineStatModificationType::Sum));
//			UpdatedStats.Stats.Add(TEXT("RoundsPlayed"), FOnlineStatUpdate(1, FOnlineStatUpdate::EOnlineStatModificationType::Sum));
//			if (bIsWinner)
//			{
//				UpdatedStats.Stats.Add(TEXT("RoundsWon"), FOnlineStatUpdate(1, FOnlineStatUpdate::EOnlineStatModificationType::Sum));
//			}
//
//			Stats->UpdateStats(UniqueId.GetUniqueNetId().ToSharedRef(), UpdatedUserStats, FOnlineStatsUpdateStatsComplete());
//		}
//	}
//}


//void ABHGamePlayerController::UpdateSaveFileOnGameEnd(bool bIsWinner)
//{
//	ABHGamePlayerState* BHGamePlayerState = Cast<ABHGamePlayerState>(PlayerState);
//	if (BHGamePlayerState)
//	{
//		// update local saved profile
//		UBHGamePersistentUser* const PersistentUser = GetPersistentUser();
//		if (PersistentUser)
//		{
//			PersistentUser->AddMatchResult(BHGamePlayerState->GetKills(), BHGamePlayerState->GetDeaths(), BHGamePlayerState->GetNumBulletsFired(), BHGamePlayerState->GetNumRocketsFired(), bIsWinner);
//			PersistentUser->SaveIfDirty();
//		}
//	}
//}

//void ABHGamePlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
//{
//	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);
//
//	if (const UWorld* World = GetWorld())
//	{
//		UBHGameGameViewportClient* BHGameViewport = Cast<UBHGameGameViewportClient>(World->GetGameViewport());
//
//		if (BHGameViewport != NULL)
//		{
//			BHGameViewport->ShowLoadingScreen();
//		}
//
//		ABHGameHUD* BHGameHUD = Cast<ABHGameHUD>(GetHUD());
//		if (BHGameHUD != nullptr)
//		{
//			// Passing true to bFocus here ensures that focus is returned to the game viewport.
//			BHGameHUD->ShowScoreboard(false, true);
//		}
//	}
//}
