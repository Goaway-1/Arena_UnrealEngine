// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABGameState.h"

AABGameMode::AABGameMode()
{
	DefaultPawnClass = AABCharacter::StaticClass();	//ABPawn의 클래스 정보를 저장 (멀티 플레이를 고려, 만들어두는 것이 아님)

	PlayerControllerClass = AABPlayerController::StaticClass();
	PlayerStateClass = AABPlayerState::StaticClass();
	GameStateClass = AABGameState::StaticClass();

	ScoreToClear = 10;
}

void AABGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	auto ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	ABPlayerState->InitPlayerData();	//초기화
}

void AABGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABGameState = Cast<AABGameState>(GameState);
}

void AABGameMode::AddScore(AABPlayerController* ScoredPlayer)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
		if((nullptr != ABPlayerController) && (ScoredPlayer == ABPlayerController))
		{
			ABPlayerController->AddGameScore();
			break;
		}
	}
	ABGameState->AddGameScore();
	
	if(GetScore() >= ScoreToClear)	//목적의 달성 종료
	{
		ABGameState->SetGameCleared();
		
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator();It;++It) (*It)->TurnOff();
		
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();It;++It)	//UI를 호출한
		{
			const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
			if(nullptr != ABPlayerController) ABPlayerController->ShowResultUI();	
		}
	}
}
int32 AABGameMode::GetScore() const
{
	return ABGameState->GetTotalGameScore();
}
