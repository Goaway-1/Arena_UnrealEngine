// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"
#include "ABGameInstance.h"
#include "ABSaveGame.h"

AABPlayerState::AABPlayerState()
{
	CharacterLevel = 1;
	GameScore = 0;
	GameHighScore = 0;
	Exp = 0;
	SaveSlotName = TEXT("Player1");
}

int32 AABPlayerState::GetGameScore() const
{
	return GameScore;
}

int32 AABPlayerState::GetCharacterLevel() const
{
	return CharacterLevel;
}

int32 AABPlayerState::GetGameHighScore() const
{
	return GameHighScore;
}

void AABPlayerState::InitPlayerData()	//초기 생성
{
	auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName,0));
	if(nullptr == ABSaveGame) ABSaveGame = GetMutableDefault<UABSaveGame>();	//없다면 저장 데이터를 새로만든다.

	//데이터를 불러온다.
	SetPlayerName(ABSaveGame->PlayerName);
	SetCharacterLevel(ABSaveGame->Level);
	GameScore = 0;
	GameHighScore = ABSaveGame->HighScore;
	Exp = ABSaveGame->Exp;
	SavePlayerData();
}

void AABPlayerState::SavePlayerData()
{
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
	NewPlayerData->PlayerName = GetPlayerName();
	NewPlayerData->Level = CharacterLevel;
	NewPlayerData->Exp = Exp;
	NewPlayerData->HighScore = GameHighScore;

	if(!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName,0)) ABLOG(Error, TEXT("SaveGame Error!"));
}

float AABPlayerState::GetExpRatio() const
{
	if(CurrentStatData->NextExp <= KINDA_SMALL_NUMBER) return 0.0f;

	float Result = (float)Exp/(float)CurrentStatData->NextExp;
	ABLOG(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"),Result,Exp,CurrentStatData->NextExp);
	return Result;
}

bool AABPlayerState::AddExp(int32 IncomeExp)
{
	if(CurrentStatData->NextExp == -1) return false; //만랩

	bool DidLevelUp = false;
	Exp += IncomeExp;
	if(Exp >= CurrentStatData->NextExp)
	{
		Exp -= CurrentStatData->NextExp;
		SetCharacterLevel(CharacterLevel + 1);
		DidLevelUp = true;
	}
	OnPlayerStateChanged.Broadcast();	//스텟바뀌었다!
	SavePlayerData();
	return DidLevelUp;
}

void AABPlayerState::AddGameScore()
{
	GameScore++;
	if(GameScore >= GameHighScore) GameHighScore = GameScore;	//최고기록갱신
	SavePlayerData();
	OnPlayerStateChanged.Broadcast();
}

void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)	//레벨업
{
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);

	CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel);
	ABCHECK(nullptr != CurrentStatData);
	
	CharacterLevel = NewCharacterLevel;
}