// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScopeBase.generated.h"

UCLASS(Abstract, Blueprintable, Transient)
class BHGAME_API AScopeBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AScopeBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USkeletalMeshComponent* ScopeMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UStaticMeshComponent* Lens;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USceneCaptureComponent2D* ScopeCam;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return ScopeMesh; }
};
