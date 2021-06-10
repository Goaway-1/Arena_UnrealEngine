// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterSelectWidget.h"
#include "ABCharacterSetting.h"
#include "ABGameInstance.h"
#include "EngineUtils.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "ABSaveGame.h"
#include "ABPlayerState.h"

void UABCharacterSelectWidget::NextCharacter(bool bForward)
{
	bForward ? CurrentIndex++ : CurrentIndex--;

	if(CurrentIndex == -1) CurrentIndex = MaxIndex - 1;
	if(CurrentIndex == MaxIndex) CurrentIndex = 0;

	auto CharacterSetting = GetDefault<UABCharacterSetting>();
	auto AssetRef = CharacterSetting->CharacterAssets[CurrentIndex];	//에셋을 불러옴

	auto ABGameInstance = GetWorld()->GetGameInstance<UABGameInstance>();
	ABCHECK(nullptr != ABGameInstance);
	ABCHECK(TargetComponent.IsValid());

	USkeletalMesh* Asset = ABGameInstance->StreamableManager.LoadSynchronous<USkeletalMesh>(AssetRef);	//생성
	if(nullptr != Asset) TargetComponent->SetSkeletalMesh(Asset);

	PrevButton = Cast<UButton>(GetWidgetFromName(TEXT("btnPrev")));
	ABCHECK(nullptr != PrevButton);
	
	NextButton = Cast<UButton>(GetWidgetFromName(TEXT("btnNext")));
	ABCHECK(nullptr != NextButton);

	TextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("edtPlayerName")));
	ABCHECK(nullptr != TextBox);

	ConfirmButton = Cast<UButton>(GetWidgetFromName(TEXT("btnConfirm")));
	ABCHECK(nullptr != ConfirmButton);

	PrevButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnPrevClicked);
	NextButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnNextClicked);
	ConfirmButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnConfirmClickecd);
}

void UABCharacterSelectWidget::OnPrevClicked()
{
	NextCharacter(false);
}

void UABCharacterSelectWidget::OnNextClicked()
{
	NextCharacter(true);
}

void UABCharacterSelectWidget::OnConfirmClickecd()
{
	FString CharacterName = TextBox->GetText().ToString();
	if(CharacterName.Len() <= 0 || CharacterName.Len() > 0) return;

	auto ABPlayerState = GetDefault<AABPlayerState>();
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
	if (UGameplayStatics::SaveGameToSlot(NewPlayerData, ABPlayerState->SaveSlotName, 0)) UGameplayStatics::OpenLevel(GetWorld(), TEXT("GamePlay"));
	else ABLOG(Error, TEXT("SaveGame Error!"));
}

void UABCharacterSelectWidget::NativeConstruct()	// UI가 뷰포트에 추가되면 호출되는 함수 (초기화)
{
	Super::NativeConstruct();

	CurrentIndex = 0;
	auto CharacterSetting = GetDefault<UABCharacterSetting>();
	MaxIndex = CharacterSetting->CharacterAssets.Num();

	for(TActorIterator<ASkeletalMeshActor> It(GetWorld());It;++It)
	{
		TargetComponent = It->GetSkeletalMeshComponent();
		break;
	}
}
