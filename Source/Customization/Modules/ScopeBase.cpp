// Fill out your copyright notice in the Description page of Project Settings.


#include "ScopeBase.h"
#include "Components/SceneCaptureComponent2D.h"

AScopeBase::AScopeBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*bReplicates=true;
	bNetUseOwnerRelevancy=true;
	*/
	ScopeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ScopeMesh"));
	RootComponent = ScopeMesh;
	ScopeMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	ScopeMesh->bReceivesDecals = true;
	ScopeMesh->CastShadow = true;
	ScopeMesh->SetCollisionObjectType(ECC_WorldDynamic);
	ScopeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScopeMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ScopeMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	ScopeMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	ScopeMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
	ScopeMesh->bOnlyOwnerSee = false;
	Lens = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lens"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LensObj(TEXT("StaticMesh'/Game/Weapons/Modular_AK/Additional_assets/Shape_Cylinder.Shape_Cylinder'"));
	if (LensObj.Succeeded())
	{
		Lens->SetStaticMesh(LensObj.Object);
		Lens->SetRelativeScale3D(FVector(0.04f, 0.04f, 0.005f));
	}
	Lens->SetupAttachment(ScopeMesh);

	ScopeCam = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("ScopeCam"));
	ScopeCam->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	ScopeCam->SetupAttachment(ScopeMesh);

	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
}

// Called when the game starts or when spawned
void AScopeBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AScopeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

