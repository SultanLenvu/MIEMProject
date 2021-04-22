// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWeapon.h"

void AMenuWeapon::OnMeshLoaded(USkeletalMeshComponent* Component, TSharedPtr<FStreamableHandle>* MeshHandle,
	FName SocketSlot)
{
	USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(MeshHandle->Get()->GetLoadedAsset());
	Component->SetSkeletalMesh(LoadedMesh);
	if (!Component->IsAttachedTo(ModuleTree[SocketSlot]) && RootComponent!=Component) Component->AttachToComponent(ModuleTree[SocketSlot],
		FAttachmentTransformRules::KeepRelativeTransform,SocketSlot);
}

// Sets default values
AMenuWeapon::AMenuWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Frame = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Frame"));
	RootComponent=Frame;
	
	Muzzel = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Muzzel"));
	/*Muzzel->SetupAttachment(Frame, TEXT("muzzel"));*/

	ForeGrip = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ForeGrip"));
	/*ForeGrip->SetupAttachment(Frame, TEXT("foregrip"));*/

	Stock = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Stock"));
	/*Stock->SetupAttachment(Frame, TEXT("stock"));*/

	Handle = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Handle"));
	/*Handle->SetupAttachment(Frame, TEXT("handle"));*/

	Magazine = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Magazine"));
	/*Magazine->SetupAttachment(Frame, TEXT("magazine"));*/

	Scope = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Scope"));
	
	Grip = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Grip"));
	
	GunPoint = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunPoint"));
	
}

// Called when the game starts or when spawned
void AMenuWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMenuWeapon::Destroyed()
{
	if (FrameHandle.IsValid()) FrameHandle->ReleaseHandle();
	if (MuzzelHandle.IsValid()) MuzzelHandle->ReleaseHandle();
	if (ForeGripHandle.IsValid()) ForeGripHandle->ReleaseHandle();
	if (StockHandle.IsValid()) StockHandle->ReleaseHandle();
	if (HandleHandle.IsValid()) HandleHandle->ReleaseHandle();
	if (GripHandle.IsValid()) GripHandle->ReleaseHandle();
	if (GunPointHandle.IsValid()) GunPointHandle->ReleaseHandle();
}


// Called every frame
void AMenuWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMenuWeapon::SetModuleMesh(ESLot Slot, FName AssetPath)
{
	USkeletalMeshComponent*& Component=Frame;
	TSharedPtr<FStreamableHandle>* LoadHandle=&FrameHandle;
	FName SlotSocket;
	switch (Slot)
	{
	case ESLot::Muzzel:
		Component=Muzzel;
		LoadHandle=&MuzzelHandle;
		SlotSocket=FName(TEXT("muzzel"));
		break;
	case ESLot::ForeGrip:
		Component=ForeGrip;
		LoadHandle=&ForeGripHandle;
		SlotSocket=FName(TEXT("foregrip"));
		break;
	case ESLot::Stock:
		Component=Stock;
		LoadHandle=&StockHandle;
		SlotSocket=FName(TEXT("stock"));
		break;
	case ESLot::Handle:
		Component=Handle;
		LoadHandle=&HandleHandle;
		SlotSocket=FName(TEXT("handle"));
		break;
	case ESLot::Magazine:
		Component=Magazine;
		LoadHandle=&MagazineHandle;
		SlotSocket=FName(TEXT("magazine"));
		break;
	case ESLot::Scope:
		Component=Scope;
		LoadHandle=&ScopeHandle;
		SlotSocket=FName(TEXT("scope"));
		break;
	case ESLot::Grip:
		Component=Grip;
		LoadHandle=&GripHandle;
		SlotSocket=FName(TEXT("grip"));
		break;
	case ESLot::GunPoint:
		Component=GunPoint;
		LoadHandle=&GunPointHandle;
		SlotSocket=FName(TEXT("gunpoint"));
		break;
	default:
		break;
	}
	if (AssetPath.IsNone()) Component->SetSkeletalMesh(nullptr);
	else
	{
		UAssetManager& AssetManager = UAssetManager::Get();
		FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
		FStreamableDelegate MeshDelegate = FStreamableDelegate::CreateUObject(this,&AMenuWeapon::OnMeshLoaded,Component,LoadHandle, SlotSocket);
		*LoadHandle = StreamableManager.RequestAsyncLoad(FSoftObjectPath(AssetPath), MeshDelegate);
	}
}

void AMenuWeapon::InitializeWeapon(TMap<ESLot, FName>& ModuleMap)
{
	for (auto& Item: ModuleMap)
	{
		SetModuleMesh(Item.Key,Item.Value);
	}
}


