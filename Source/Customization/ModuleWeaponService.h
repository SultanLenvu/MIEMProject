// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Runtime/Engine/Classes/Engine/DataTable.h>
#include "ModuleWeaponService.generated.h"

USTRUCT(BlueprintType)
struct FHandlesDataRow : public FTableRowBase
{
	GENERATED_BODY()
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Description;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName AssetPath;
};

USTRUCT(BlueprintType)
struct FFramesDataRow : public FTableRowBase
{
	GENERATED_BODY()
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Description;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName AssetPath;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName HandleSocketName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName ScopeSocketName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName StockSocketName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName MuzzelSocketName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName ForeGripSocketName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName MagazineSocketName;
};

USTRUCT(BlueprintType)
struct FStocksDataRow : public FTableRowBase
{
	GENERATED_BODY()
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Description;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName AssetPath;
};

USTRUCT(BlueprintType)
struct FMuzzelsDataRow : public FTableRowBase
{
	GENERATED_BODY()
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Description;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName AssetPath;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName GunPointSocketName;
};

USTRUCT(BlueprintType)
struct FMagazinesDataRow : public FTableRowBase
{
	GENERATED_BODY()
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Description;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName AssetPath;
};

USTRUCT(BlueprintType)
struct FForeGripsDataRow : public FTableRowBase
{
	GENERATED_BODY()
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Description;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName AssetPath;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName GripSocketName;
};

USTRUCT(BlueprintType)
struct FGripsDataRow : public FTableRowBase
{
	GENERATED_BODY()
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Description;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName AssetPath;
};

USTRUCT(BlueprintType)
struct FGunPointsDataRow : public FTableRowBase
{
	GENERATED_BODY()
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Description;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName AssetPath;
};

USTRUCT(BlueprintType)
struct FScopesDataRow : public FTableRowBase
{
	GENERATED_BODY()
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Description;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName AssetPath;
};

UCLASS()
class BHGAME_API UModuleWeaponService : public UObject
{
	GENERATED_BODY()
	
		UPROPERTY()
		UDataTable* FrameTable;
	UPROPERTY()
		UDataTable* MuzzelTable;
	UPROPERTY()
		UDataTable* ForeGripTable;
	UPROPERTY()
		UDataTable* HandleTable;
	UPROPERTY()
		UDataTable* StockTable;
	UPROPERTY()
		UDataTable* MagazineTable;
	UPROPERTY()
		UDataTable* GunPointTable;
	UPROPERTY()
		UDataTable* ScopeTable;
	UPROPERTY()
		UDataTable* GripTable;
public:
	UModuleWeaponService();
	void GetFrames(TArray<FFramesDataRow*>& Frames) const;
	void GetForeGrips(TArray<FForeGripsDataRow*>& ForeGrips) const;
	void GetMuzzels(TArray<FMuzzelsDataRow*>& Muzzels) const;
	void GetStocks(TArray<FStocksDataRow*>& Stocks) const;
	void GetHandles(TArray<FHandlesDataRow*>& Handles) const;
	void GetMagazines(TArray<FMagazinesDataRow*>& Magazines) const;
	void GetScopes(TArray<FScopesDataRow*>& Scopes) const;
	void GetGunPoints(TArray<FGunPointsDataRow*>& GunPoints) const;
	void GetGrips(TArray<FGripsDataRow*>& Grips) const;

	const FFramesDataRow* GetFrame(const FName& RowName) const;
	const FForeGripsDataRow* GetForeGrip(const FName& RowName) const;
	const FMuzzelsDataRow* GetMuzzel(const FName& RowName) const;
	const FStocksDataRow* GetStock(const FName& RowName) const;
	const FHandlesDataRow* GetHandle(const FName& RowName) const;
	const FScopesDataRow* GetScope(const FName& RowName) const;
	const FMagazinesDataRow* GetMagazine(const FName& RowName) const;
	const FGunPointsDataRow* GetGunPoint(const FName& RowName) const;
	const FGripsDataRow* GetGrip(const FName& RowName) const;
	~UModuleWeaponService();
};
