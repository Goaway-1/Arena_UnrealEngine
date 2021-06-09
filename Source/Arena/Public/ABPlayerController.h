// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Arena.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARENA_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AABPlayerController();
	
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* APawn) override;

	class UABHUDWidget* GetHUDWidget() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=UI)
	TSubclassOf<class UABHUDWidget> HUDWidgetClass;

private:
	UPROPERTY()
	class UABHUDWidget* HUDWidget;
};
