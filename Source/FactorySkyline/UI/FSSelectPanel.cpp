// ILikeBanas


#include "FactorySkyline/UI/FSSelectPanel.h"
#include "FactorySkyline/UI/FSFoldWidgetBase.h"
#include "FactorySkyline/FSkyline.h"
#include "FactorySkyline/FSController.h"
#include "FactorySkyline/FSDesign.h"
#include "Components/CheckBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"


UFSSelectPanel::UFSSelectPanel(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UFSSelectPanel::onCheckBoxChanged(bool IsChecked)
{
	if (IsChecked) this->Expand();
	else this->Fold();
}

void UFSSelectPanel::onStartSelect()
{
	AFSkyline* Skyline = AFSkyline::Get(this);
	Skyline->FSCtrl->StartSelectMode();
}

void UFSSelectPanel::Init()
{
	FScriptDelegate Func1;
	Func1.BindUFunction(this, FName("onCheckBoxChanged"));
	this->FoldPanel->FoldBox->OnCheckStateChanged.Add(Func1);

	Func1.BindUFunction(this, FName("onStartSelect"));
	this->SelectButton->OnClicked.Add(Func1);

	this->FoldPanel->Title->SetText(FText::FromString(FString(TEXT("Select"))));

	this->FoldPanel->FoldBox->SetCheckedState(ECheckBoxState::Unchecked);
	this->onCheckBoxChanged(false);

	Func1.BindUFunction(this, FName("onCheckBoxAddChanged"));
	this->CheckBoxAdd->OnCheckStateChanged.Add(Func1);
	this->CheckBoxAdd->SetCheckedState(ECheckBoxState::Unchecked);

}

void UFSSelectPanel::Load(UFSDesign* Design)
{
	this->Design = Design;

	if (this->Design->AddConstructedBuildable) {
		this->CheckBoxAdd->SetCheckedState(ECheckBoxState::Checked);
	} else {
		this->CheckBoxAdd->SetCheckedState(ECheckBoxState::Unchecked);
	}
}

void UFSSelectPanel::Fold()
{
	//this->FoldPanel->Dark->SetVisibility(ESlateVisibility::Collapsed);
	this->Readme->SetVisibility(ESlateVisibility::Collapsed);
}

void UFSSelectPanel::Expand()
{
	//this->FoldPanel->Dark->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->Readme->SetVisibility(ESlateVisibility::Visible);
}

void UFSSelectPanel::onCheckBoxAddChanged(bool IsChecked)
{
	AFSkyline* Skyline = AFSkyline::Get(this);
	//Skyline->FSCtrl->FullPreview = IsChecked;
	Skyline->FSCtrl->Design->AddConstructedBuildable = IsChecked;
}