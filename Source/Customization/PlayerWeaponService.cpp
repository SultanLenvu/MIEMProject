// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerWeaponService.h"

FPlayerWeaponDataRow* UPlayerWeaponService::GetWeaponByName(const FName& Name)
{
	FString ContextString(TEXT("Getting a particular weapon"));
	CurrentModifyingRow=WeaponsTable->FindRow<FPlayerWeaponDataRow>(Name, ContextString);
	return CurrentModifyingRow;
}

bool UPlayerWeaponService::SetNewModule(FName NewModuleRowName, ESLot Slot)
{
	switch (Slot)
	{
	case ESLot::Muzzel:
		CurrentModifyingRow->Muzzel= NewModuleRowName;
		break;
	case ESLot::ForeGrip:
		CurrentModifyingRow->ForeGrip= NewModuleRowName;
		break;
	case ESLot::Stock:
		CurrentModifyingRow->Stock= NewModuleRowName;
		break;
	case ESLot::Handle:
		CurrentModifyingRow->Handle= NewModuleRowName;
		break;
	case ESLot::Magazine:
		CurrentModifyingRow->Magazine= NewModuleRowName;
		break;
	case ESLot::Scope:
		CurrentModifyingRow->Scope= NewModuleRowName;
		break;
	case ESLot::Grip:
		CurrentModifyingRow->Grip= NewModuleRowName;
		break;
	case ESLot::GunPoint:
		CurrentModifyingRow->GunPoint= NewModuleRowName;
		break;
	default:
		break;
	}
	return true;
	
}


UPlayerWeaponService::UPlayerWeaponService()
{
	const ConstructorHelpers::FObjectFinder<UDataTable> WeaponsTableObj(TEXT("DataTable'/Game/DataTables/CuztomizationTables_-_PlayerWeapons.CuztomizationTables_-_PlayerWeapons'"));
	if (WeaponsTableObj.Object)
	{
		WeaponsTable = WeaponsTableObj.Object;
	}
}

void UPlayerWeaponService::GetWeapons(TArray<FPlayerWeaponDataRow*>& Weapons)
{
	const FString ContextString(TEXT("Gettting a list of weapons"));
	WeaponsTable->GetAllRows<FPlayerWeaponDataRow>(ContextString, Weapons);
}

UPlayerWeaponService::~UPlayerWeaponService()
{
}
