// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/Actor.h"
#include "Engine/AssetManager.h"
#include "MenuWeapon.generated.h"

UENUM(BlueprintType)
enum ESLot
{
	Frame,
	Muzzel,
	ForeGrip,
	Stock,
	Scope,
	Magazine,
	Grip,
	Handle,
	GunPoint,
	None
};

UCLASS(BlueprintType)
class BHGAME_API AMenuWeapon : public AActor
{
	GENERATED_BODY()

	void OnMeshLoaded(USkeletalMeshComponent* Component, TSharedPtr<FStreamableHandle>* MeshHandle,FName SocketSlot);
	TSharedPtr<FStreamableHandle> FrameHandle;
	TSharedPtr<FStreamableHandle> MuzzelHandle;
	TSharedPtr<FStreamableHandle> ForeGripHandle;
	TSharedPtr<FStreamableHandle> StockHandle;
	TSharedPtr<FStreamableHandle> MagazineHandle;
	TSharedPtr<FStreamableHandle> HandleHandle;
	TSharedPtr<FStreamableHandle> GripHandle;
	TSharedPtr<FStreamableHandle> ScopeHandle;
	TSharedPtr<FStreamableHandle> GunPointHandle;

	UPROPERTY()
	TMap<FName,USkeletalMeshComponent*> ModuleTree{
		{TEXT("muzzel"),Frame},
		{TEXT("foregrip"),Frame},
		{TEXT("stock"),Frame},
		{TEXT("magazine"),Frame},
		{TEXT("handle"),Frame},
		{TEXT("scope"),Frame},
		{TEXT("gunpoint"),Muzzel},
		{TEXT("grip"),ForeGrip},
	};
public:	
	AMenuWeapon();
	const FString FrameSlot =FString(TEXT("FRAME"));
	const FString MuzzelSlot =FString(TEXT("MUZZEL"));
	 const FString ForeGripSlot =FString(TEXT("FOREGRIP"));
	 const FString StockSlot =FString(TEXT("STOCK"));
	 const FString HandleSlot =FString(TEXT("HANDLE"));
	 const FString MagazineSlot =FString(TEXT("MAGAZINE"));
	 const FString ScopeSlot =FString(TEXT("SCOPE"));
	 const FString GripSlot =FString(TEXT("GRIP"));
	 const FString GunPointSlot =FString(TEXT("GUNPOINT"));
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Modules")
	USkeletalMeshComponent* Frame;
	UPROPERTY(EditDefaultsOnly, Category="Modules")
	USkeletalMeshComponent* Muzzel;
	UPROPERTY(EditDefaultsOnly, Category="Modules")
	USkeletalMeshComponent* ForeGrip;
	UPROPERTY(EditDefaultsOnly, Category="Modules")
    USkeletalMeshComponent* Grip;
	UPROPERTY(EditDefaultsOnly, Category="Modules")
	USkeletalMeshComponent* Magazine;
	UPROPERTY(EditDefaultsOnly, Category="Modules")
	USkeletalMeshComponent* Handle;
	UPROPERTY(EditDefaultsOnly, Category="Modules")
	USkeletalMeshComponent* Stock;
	UPROPERTY(EditDefaultsOnly, Category="Modules")
	USkeletalMeshComponent* Scope;
	UPROPERTY(EditDefaultsOnly, Category="Modules")
	USkeletalMeshComponent* GunPoint;
	virtual void Destroyed() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetModuleMesh(ESLot Slot, FName AssetPath);
	void InitializeWeapon(TMap<ESLot,FName>& ModuleMap);
};
