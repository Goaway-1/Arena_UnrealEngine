// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Arena.h"
#include "Blueprint/UserWidget.h"
#include "ABGameplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENA_API UABGameplayWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override; //UI위젯이 초기화되는 시점 -> 이름으로 버튼 찾을거

	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnReturnToTitleClicked();

	UFUNCTION()
	void OnRetryGameClicked();

	UPROPERTY()
	class UButton* ResumeButton;

	UPROPERTY()
	class UButton* ReturnToTitleButton;
	
	UPROPERTY()
	class UButton* RetryGameButton;
};
