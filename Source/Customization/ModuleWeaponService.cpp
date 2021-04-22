// Fill out your copyright notice in the Description page of Project Settings.


#include "ModuleWeaponService.h"

UModuleWeaponService::UModuleWeaponService()
{
	const ConstructorHelpers::FObjectFinder<UDataTable> FrameTableObj(TEXT("DataTable'/Game/DataTables/CuztomizationTables_-_Frames.CuztomizationTables_-_Frames'"));
	if (FrameTableObj.Object)
	{
		FrameTable = FrameTableObj.Object;
	}
	const ConstructorHelpers::FObjectFinder<UDataTable> HandleTableObj(TEXT("DataTable'/Game/DataTables/CuztomizationTables_-_Handles.CuztomizationTables_-_Handles'"));
	if (HandleTableObj.Object)
	{
		HandleTable = HandleTableObj.Object;
	}
	const ConstructorHelpers::FObjectFinder<UDataTable> StockTableObj(TEXT("DataTable'/Game/DataTables/CuztomizationTables_-_Stocks.CuztomizationTables_-_Stocks'"));
	if (StockTableObj.Object)
	{
		StockTable = StockTableObj.Object;
	}
	const ConstructorHelpers::FObjectFinder<UDataTable> MuzzelTableObj(TEXT("DataTable'/Game/DataTables/CuztomizationTables_-_Muzzels.CuztomizationTables_-_Muzzels'"));
	if (MuzzelTableObj.Object)
	{
		MuzzelTable = MuzzelTableObj.Object;
	}
	const ConstructorHelpers::FObjectFinder<UDataTable> ForeGripTableObj(TEXT("DataTable'/Game/DataTables/CuztomizationTables_-_ForeGrips.CuztomizationTables_-_ForeGrips'"));
	if (ForeGripTableObj.Object)
	{
		ForeGripTable = ForeGripTableObj.Object;
	}
	const ConstructorHelpers::FObjectFinder<UDataTable> MagazineTableObj(TEXT("DataTable'/Game/DataTables/CuztomizationTables_-_Magazines.CuztomizationTables_-_Magazines'"));
	if (MagazineTableObj.Object)
	{
		MagazineTable = MagazineTableObj.Object;
	}
	const ConstructorHelpers::FObjectFinder<UDataTable> GripTableObj(TEXT("DataTable'/Game/DataTables/CuztomizationTables_-_Grips.CuztomizationTables_-_Grips'"));
	if (GripTableObj.Object)
	{
		GripTable = GripTableObj.Object;
	}
	const ConstructorHelpers::FObjectFinder<UDataTable> GunPointTableObj(TEXT("DataTable'/Game/DataTables/CuztomizationTables_-_GunPoints.CuztomizationTables_-_GunPoints'"));
	if (GunPointTableObj.Object)
	{
		GunPointTable = GunPointTableObj.Object;
	}
	const ConstructorHelpers::FObjectFinder<UDataTable> ScopeTableObj(TEXT("DataTable'/Game/DataTables/CuztomizationTables_-_Scopes.CuztomizationTables_-_Scopes'"));
	if (ScopeTableObj.Object)
	{
		ScopeTable = ScopeTableObj.Object;
	}
}

void UModuleWeaponService::GetFrames(TArray<FFramesDataRow*>& Frames) const
{
	const FString ContextString(TEXT("Getting list of Frames"));
	FrameTable->GetAllRows<FFramesDataRow>(ContextString, Frames);
}

void UModuleWeaponService::GetForeGrips(TArray<FForeGripsDataRow*>& ForeGrips) const
{
	const FString ContextString(TEXT("Getting list of ForeGrips"));
	ForeGripTable->GetAllRows<FForeGripsDataRow>(ContextString, ForeGrips);
}

void UModuleWeaponService::GetMuzzels(TArray<FMuzzelsDataRow*>& Muzzels) const
{
	const FString ContextString(TEXT("Getting list of Muzzles"));
	MuzzelTable->GetAllRows<FMuzzelsDataRow>(ContextString, Muzzels);
}

void UModuleWeaponService::GetStocks(TArray<FStocksDataRow*>& Stocks) const
{
	const FString ContextString(TEXT("Getting list of Stocks"));
	StockTable->GetAllRows<FStocksDataRow>(ContextString, Stocks);
}

void UModuleWeaponService::GetHandles(TArray<FHandlesDataRow*>& Handles) const
{
	const FString ContextString(TEXT("Getting list of Handles"));
	HandleTable->GetAllRows<FHandlesDataRow>(ContextString, Handles);
}

void UModuleWeaponService::GetMagazines(TArray<FMagazinesDataRow*>& Magazines) const
{
	const FString ContextString(TEXT("Getting list of Magazines"));
	MagazineTable->GetAllRows<FMagazinesDataRow>(ContextString, Magazines);
}

void UModuleWeaponService::GetScopes(TArray<FScopesDataRow*>& Scopes) const
{
	const FString ContextString(TEXT("Getting list of Scopes"));
	ScopeTable->GetAllRows<FScopesDataRow>(ContextString, Scopes);
}

void UModuleWeaponService::GetGunPoints(TArray<FGunPointsDataRow*>& GunPoints) const
{
	const FString ContextString(TEXT("Getting list of GunPoints"));
	GunPointTable->GetAllRows<FGunPointsDataRow>(ContextString, GunPoints);
}

void UModuleWeaponService::GetGrips(TArray<FGripsDataRow*>& Grips) const
{
	const FString ContextString(TEXT("Getting list of Grips"));
	GripTable->GetAllRows<FGripsDataRow>(ContextString, Grips);
}

const FFramesDataRow* UModuleWeaponService::GetFrame(const FName& RowName) const
{
	const FString ContextString(TEXT("Getting a particular row"));
	return FrameTable->FindRow<FFramesDataRow>(RowName, ContextString);
}

const FForeGripsDataRow* UModuleWeaponService::GetForeGrip(const FName& RowName) const
{
	const FString ContextString(TEXT("Getting a particular foregrip"));
	return ForeGripTable->FindRow<FForeGripsDataRow>(RowName, ContextString);
}

const FMuzzelsDataRow* UModuleWeaponService::GetMuzzel(const FName& RowName) const
{
	const FString ContextString(TEXT("Getting a particular muzzel"));
	return MuzzelTable->FindRow<FMuzzelsDataRow>(RowName, ContextString);
}

const FStocksDataRow* UModuleWeaponService::GetStock(const FName& RowName) const
{
	const FString ContextString(TEXT("Getting a particular stock"));
	return StockTable->FindRow<FStocksDataRow>(RowName, ContextString);
}

const FHandlesDataRow* UModuleWeaponService::GetHandle(const FName& RowName) const
{
	const FString ContextString(TEXT("Getting a particular handle"));
	return HandleTable->FindRow<FHandlesDataRow>(RowName, ContextString);
}

const FScopesDataRow* UModuleWeaponService::GetScope(const FName& RowName) const
{
	const FString ContextString(TEXT("Getting a particular scope"));
	return ScopeTable->FindRow<FScopesDataRow>(RowName, ContextString);
}

const FMagazinesDataRow* UModuleWeaponService::GetMagazine(const FName& RowName) const
{
	const FString ContextString(TEXT("Getting a particular magazine"));
	return MagazineTable->FindRow<FMagazinesDataRow>(RowName, ContextString);
}

const FGunPointsDataRow* UModuleWeaponService::GetGunPoint(const FName& RowName) const
{
	const FString ContextString(TEXT("Getting a particular gunpoint"));
	return GunPointTable->FindRow<FGunPointsDataRow>(RowName, ContextString);
}

const FGripsDataRow* UModuleWeaponService::GetGrip(const FName& RowName) const
{
	const FString ContextString(TEXT("Getting a particular grip"));
	return GripTable->FindRow<FGripsDataRow>(RowName, ContextString);
}


UModuleWeaponService::~UModuleWeaponService()
{

}