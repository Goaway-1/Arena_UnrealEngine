// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Arena.h"
#include "GameFramework/PlayerState.h"
#include "ABPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerStateChangedDelegate);

UCLASS()
class ARENA_API AABPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AABPlayerState();

	int32 GetGameScore() const;
	
	//게임 데이터 저장과 로딩
	int32 GetGameHighScore() const;
	FString SaveSlotName;
	
	int32 GetCharacterLevel() const;
	int32 GetCharacterIndex() const;
	float GetExpRatio() const;
	bool AddExp(int32 IncomeExp);
	void AddGameScore();
	
	void InitPlayerData();
	void SavePlayerData();
	
	FOnPlayerStateChangedDelegate OnPlayerStateChanged;
protected:
	UPROPERTY(Transient)
	int32 GameScore;	//점수

	UPROPERTY(Transient)
	int32 GameHighScore;
	
	UPROPERTY(Transient)
	int32 CharacterLevel;	//레벨

	UPROPERTY(Transient)
	int32 Exp;

	UPROPERTY(Transient)
	int32 CharacterIndex;

private:
	void SetCharacterLevel(int32 NewCharacterLevel);

	struct FABCharacterData* CurrentStatData;
};
