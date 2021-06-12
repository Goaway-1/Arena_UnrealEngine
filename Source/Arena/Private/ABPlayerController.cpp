// Fill out your copyright notice in the Description page of Project Settings.

#include "ABPlayerController.h"
#include "ABHUDWidget.h"
#include "ABPlayerState.h"
#include "ABCharacter.h"
#include "ABGameplayResultWidget.h"
#include "ABGameplayWidget.h"
#include "ABGameState.h"

AABPlayerController::AABPlayerController()
{
	static ConstructorHelpers::FClassFinder<UABHUDWidget>
	UI_HUD_C(TEXT("/Game/Book/UI/UI_HUD.UI_HUD_C"));
	if(UI_HUD_C.Succeeded()) HUDWidgetClass = UI_HUD_C.Class;

	static ConstructorHelpers::FClassFinder<UABGameplayWidget>
	UI_MENU_C(TEXT("/Game/Book/UI/UI_Menu.UI_Menu_C"));
	if(UI_MENU_C.Succeeded()) MenuWidgetClass = UI_MENU_C.Class;

	static ConstructorHelpers::FClassFinder<UABGameplayResultWidget>
	UI_RESULT_C(TEXT("/Game/Book/UI/UI_Result.UI_Result_C"));
	if(UI_RESULT_C.Succeeded()) ResultWidgetClass = UI_RESULT_C.Class;
}

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPlayerController::OnPossess(APawn* APawn)
{
	ABLOG_S(Warning);
	Super::OnPossess(APawn);
}

void AABPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction(TEXT("GamePause"),EInputEvent::IE_Pressed, this, &AABPlayerController::OnGamePause);	//눌리는지 확인
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ChangeInputMode(true);	//입력모드를 싸움

	// FInputModeGameOnly InputMode;
	// SetInputMode(InputMode);

	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
	ABCHECK(nullptr != HUDWidget);
	HUDWidget->AddToViewport(1);

	ResultWidget = CreateWidget<UABGameplayResultWidget>(this, ResultWidgetClass);
	ABCHECK(nullptr != ResultWidget);
	
	//연결
	ABPlayerState = Cast<AABPlayerState>(PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	HUDWidget->BindPlayerState(ABPlayerState);
	ABPlayerState->OnPlayerStateChanged.Broadcast();
}

UABHUDWidget * AABPlayerController::GetHUDWidget() const
{
	return HUDWidget;
}

void AABPlayerController::AddGameScore() const
{
	ABPlayerState->AddGameScore();	
}

void AABPlayerController::NPCKill(AABCharacter* KilledNPC) const
{
	ABPlayerState->AddExp(KilledNPC->GetExp());
}

void AABPlayerController::ShowResultUI()
{
	auto ABGameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this));
	ABCHECK(nullptr != ABGameState);
	ResultWidget->BindGameState(ABGameState);
	
	ResultWidget->AddToViewport();
	ChangeInputMode(false);
}

void AABPlayerController::ChangeInputMode(bool bGameMode)
{
	if(bGameMode)	//게임에 입력
	{
		SetInputMode(GameInputMode);	
		bShowMouseCursor = false;
	}
	else
	{
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;	//마우스 커서 띄움
	}
}

void AABPlayerController::OnGamePause()
{
	MenuWidget = CreateWidget<UABGameplayWidget>(this, MenuWidgetClass);	//만들어
	ABCHECK(nullptr != MenuWidget);
	MenuWidget->AddToViewport(3); //화면에 추가
	
	SetPause(true);
	ChangeInputMode(false);	//입력모드 바꿈
}