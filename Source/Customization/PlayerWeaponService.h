// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWeapon.h"
#include "Engine/DataTable.h"
#include "PlayerWeaponService.generated.h"


USTRUCT()
struct FPlayerWeaponDataRow : public FTableRowBase
{
	GENERATED_BODY()
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Description;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Frame;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Stock;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Handle;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Magazine;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Muzzel;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName ForeGrip;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName GunPoint;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Scope;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Grip;
};

UCLASS()
class BHGAME_API UPlayerWeaponService : public UObject
{
	GENERATED_BODY()
	
	UPROPERTY()
	UDataTable* WeaponsTable;
	FPlayerWeaponDataRow* CurrentModifyingRow;
public:
	UPlayerWeaponService();
	void GetWeapons(TArray<FPlayerWeaponDataRow*>& Weapons);
	FPlayerWeaponDataRow* GetWeaponByName(const FName& Name);
	bool SetNewModule(FName NewModuleRowName, ESLot Slot);
	~UPlayerWeaponService();
};
