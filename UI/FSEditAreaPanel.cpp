// ILikeBanas


#include "FactorySkyline/UI/FSEditAreaPanel.h"
#include "FactorySkyline/FSkyline.h"


UFSEditAreaPanel::UFSEditAreaPanel(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UFSEditAreaPanel::onCheckBoxChanged(bool IsChecked)
{
	if (IsChecked) this->Expand();
	else this->Fold();
}

void UFSEditAreaPanel::onFullPreviewChanged(bool IsChecked)
{
	AFSkyline* Skyline = AFSkyline::Get(this);
	Skyline->FSCtrl->FullPreview = IsChecked;
}

void UFSEditAreaPanel::onSetItem()
{
	AFSkyline* Skyline = AFSkyline::Get(this);
	Skyline->FSCtrl->StartSetAreaAnchorMode();
}

void UFSEditAreaPanel::onStartCopy()
{
	AFSkyline* Skyline = AFSkyline::Get(this);
	Skyline->FSCtrl->Select->SelectBuildablesInAreaBox();
}

void UFSEditAreaPanel::onChangeAreaBox()
{
	
	AFSkyline* Skyline = AFSkyline::Get(this);
	float num = FCString::Atof(*Count->GetText().ToString());
	Skyline->FSCtrl->Select->ChangeAreaBox(num);
	
}

void UFSEditAreaPanel::Init()
{
	FScriptDelegate Func1;
	//Func1.BindUFunction(this, FName("onCheckBoxChanged"));
	//this->FoldPanel->FoldBox->OnCheckStateChanged.Add(Func1);

	Func1.BindUFunction(this, FName("onSetItem"));
	this->SetItemButton->OnClicked.Add(Func1);

	Func1.BindUFunction(this, FName("onStartCopy"));
	this->StartCopyButton->OnClicked.Add(Func1);

	Func1.BindUFunction(this, FName("onChangeAreaBox"));
	this->ChangeSelectionBoxButton->OnClicked.Add(Func1);

	//this->FoldPanel->Title->SetText(FText::FromString(FString(TEXT("Basic Copy"))));


	//this->FoldPanel->FoldBox->SetCheckedState(ECheckBoxState::Unchecked);
	this->onCheckBoxChanged(false);

}

void UFSEditAreaPanel::Load(UFSDesign* Design)
{
	this->Design = Design;
	//this->Design->SetItemFeedback = this->SetItemStatus;
	//AFSkyline::Get(this)->FSCtrl->CheckAnchor(false);
}

void UFSEditAreaPanel::Fold()
{
}

void UFSEditAreaPanel::Expand()
{
}