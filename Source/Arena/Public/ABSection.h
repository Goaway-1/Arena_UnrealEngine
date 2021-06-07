// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Arena.h"
#include "GameFramework/Actor.h"
#include "ABSection.generated.h"

UCLASS()
class ARENA_API AABSection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABSection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, Category=Mesh, Meta = (AllowPrivateAccess = true))
	TArray<UStaticMeshComponent*> GateMeshes;	//배열 여러개

	UPROPERTY(VisibleAnywhere, Category=Trigger, Meta = (AllowPrivateAccess = true))
	TArray<UBoxComponent*> GateTriggers;	

private:
	UPROPERTY(VisibleAnywhere, Category=Mesh, Meta=(AllowPrivateAccess = true))
	UStaticMeshComponent *Mesh;
	
	UPROPERTY(VisibleAnywhere, Category=Trigger, Meta=(AllowPrivateAccess = true))
    UBoxComponent *Trigger;
};
