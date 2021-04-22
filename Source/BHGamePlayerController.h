#pragma once

#include "BHGameGameMode.h"
//#include "BHGameLeaderboards.h"
#include "BHGamePlayerController.generated.h"

class ABHGameHUD;

UCLASS(config = Game)
class ABHGamePlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

public:
	///** sets spectator location and rotation */
	//UFUNCTION(reliable, client)
	//	void ClientSetSpectatorCamera(FVector CameraLocation, FRotator CameraRotation);

	///** notify player about started match */
	//UFUNCTION(reliable, client)
	//	void ClientGameStarted();

	///** Starts the online game using the session name in the PlayerState */
	//UFUNCTION(reliable, client)
	//	void ClientStartOnlineGame();

	///** Ends the online game using the session name in the PlayerState */
	//UFUNCTION(reliable, client)
	//	void ClientEndOnlineGame();

	///** notify player about finished match */
	//virtual void ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner);

	///** Notifies clients to send the end-of-round event */
	//UFUNCTION(reliable, client)
	//	void ClientSendRoundEndEvent(bool bIsWinner, int32 ExpendedTimeInSeconds);

	/** used for input simulation from blueprint (for automatic perf tests) */
	//UFUNCTION(BlueprintCallable, Category = "Input")
	//	void SimulateInputKey(FKey Key, bool bPressed = true);

	/* Overriden Message implementation. */
	//virtual void ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime) override;

	/* Tell the HUD to toggle the chat window. */
	//void ToggleChatWindow();

	/** Local function say a string */
	//UFUNCTION(exec)
	//	virtual void Say(const FString& Msg);
	
	/** RPC for clients to talk to server */
	//UFUNCTION(unreliable, server, WithValidation)
	//	void ServerSay(const FString& Msg);

	/** notify local client about deaths */
	void OnDeathMessage(class ABHGamePlayerState* KillerPlayerState, class ABHGamePlayerState* KilledPlayerState, const UDamageType* KillerDamageType);

	/** toggle InGameMenu handler */
	//void OnToggleInGameMenu();

	/** Hides scoreboard if currently diplayed */
	//void OnConditionalCloseScoreboard();

	/** Toggles scoreboard */
	//void OnToggleScoreboard();

	/** shows scoreboard */
	//void OnShowScoreboard();

	/** hides scoreboard */
	//void OnHideScoreboard();

	/** set health regen cheat */
	void SetHealthRegen(bool bEnable);

	/** get health regen cheat */
	bool HasHealthRegen() const;

	/** check if gameplay related actions (movement, weapon usage, etc) are allowed right now */
	bool IsGameInputAllowed() const;

	/** is game menu currently active? */
	//bool IsGameMenuVisible() const;

	/** Ends and/or destroys game session */
	//void CleanupSessionOnReturnToMenu();

	// Begin APlayerController interface

	/** Returns true if movement input is ignored. Overridden to always allow spectators to move. */
	virtual bool IsMoveInputIgnored() const override;

	/** Returns true if look input is ignored. Overridden to always allow spectators to look around. */
	virtual bool IsLookInputIgnored() const override;

	// End APlayerController interface

	/** Returns a pointer to the BHGame game hud. May return NULL. */
	ABHGameHUD* GetBHGameHUD() const;

	/** Informs that player fragged someone */
	void OnKill();

	/** Cleans up any resources necessary to return to main menu.  Does not modify GameInstance state. */
	//virtual void HandleReturnToMainMenu();

	/** Associate a new UPlayer with this PlayerController. */
	virtual void SetPlayer(UPlayer* Player);

	// end ABHGamePlayerController-specific

	//virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

protected:
	/** health regen cheat */
	UPROPERTY(Transient, Replicated)
		uint8 bHealthRegen : 1;

	/** should produce force feedback? */
	uint8 bIsVibrationEnabled : 1;

	/** if set, gameplay related actions (movement, weapn usage, etc) are allowed */
	uint8 bAllowGameActions : 1;

	/** true for the first frame after the game has ended */
	uint8 bGameEndedFrame : 1;

	/** stores pawn location at last player death, used where player scores a kill after they died **/
	FVector LastDeathLocation;

	/** BHGame in-game menu */
	TSharedPtr<class FBHGameIngameMenu> BHGameIngameMenu;

	/** try to find spot for death cam */
	bool FindDeathCameraSpot(FVector& CameraLocation, FRotator& CameraRotation);

	virtual void BeginDestroy() override;

	//Begin AActor interface

	/** after all game elements are created */
	virtual void PostInitializeComponents() override;

	/* Flag to prevent duplicate input bindings when using the same player controller for multiple maps */
	bool bHasInitializedInputComponent;

public:
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	//End AActor interface

	//Begin AController interface

	/** transition to dead state, retries spawning later */
	virtual void FailedToSpawnPawn() override;

	/** update camera when pawn dies */
	virtual void PawnPendingDestroy(APawn* P) override;

	//End AController interface

	// Begin APlayerController interface

	/** respawn after dying */
	virtual void UnFreeze() override;

	/** sets up input */
	virtual void SetupInputComponent() override;

	/**
	 * Called from game info upon end of the game, used to transition to proper state.
	 *
	 * @param EndGameFocus Actor to set as the view target on end game
	 * @param bIsWinner true if this controller is on winning team
	 */
	//virtual void GameHasEnded(class AActor* EndGameFocus = NULL, bool bIsWinner = false) override;
	
	/** Return the client to the main menu gracefully.  ONLY sets GI state. */
	//void ClientReturnToMainMenu_Implementation(const FString& ReturnReason) override;

	/** Causes the player to commit suicide */
	UFUNCTION(exec)
		virtual void Suicide();

	/** Notifies the server that the client has suicided */
	UFUNCTION(reliable, server, WithValidation)
		void ServerSuicide();

	// End APlayerController interface

	//FName	ServerSayString;

	// Timer used for updating friends in the player tick.
	//float ShooterFriendUpdateTimer;

	/** enable analog fire trigger mode */
	UPROPERTY(config)
		bool bAnalogFireTrigger;

	/** threshold trigger fires */
	UPROPERTY(config)
		float FireTriggerThreshold;

private:
	/** Handle for efficient management of ClientStartOnlineGame timer */
	//FTimerHandle TimerHandle_ClientStartOnlineGame;
};


