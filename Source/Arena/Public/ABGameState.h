// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Arena.h"
#include "GameFramework/GameStateBase.h"
#include "ABGameState.generated.h"

/**
 * 
 */
UCLASS()
class ARENA_API AABGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AABGameState();

	int32 GetTotalGameScore() const;
	void AddGameScore();

private:
	UPROPERTY(Transient)
	int32 TotalGameScore;
};
