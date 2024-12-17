// ILikeBanas


#include "FactorySkyline/UI/FSOperatingWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "FactorySkyline/FSDesign.h"
#include "FactorySkyline/FSkyline.h"


UFSOperatingWidget::UFSOperatingWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UFSOperatingWidget::Init()
{
	FScriptDelegate Func1;
	Func1.BindUFunction(this, FName("OnTab1Click"));
	Tab1->OnClicked.Add(Func1);
	FScriptDelegate Func2;
	Func2.BindUFunction(this, FName("OnTab2Click"));
	Tab2->OnClicked.Add(Func2);
	FScriptDelegate Func3;
	Func3.BindUFunction(this, FName("OnTab3Click"));
	Tab3->OnClicked.Add(Func3);
	FScriptDelegate Func4;
	Func4.BindUFunction(this, FName("OnTab4Click"));
	Tab4->OnClicked.Add(Func4);
	FScriptDelegate FuncSelect;
	
	FSlateBrush Normal;
	Normal.TintColor = FSlateColor(FLinearColor(1.0, 1.0, 1.0, 0.0));
	Normal.DrawAs = ESlateBrushDrawType::Box;
	Normal.Margin = FMargin(0.25);
	FSlateBrush NormalClick;
	NormalClick.TintColor = FSlateColor(FLinearColor(0.341176, 0.341176, 0.341176));
	NormalClick.DrawAs = ESlateBrushDrawType::Box;
	NormalClick.Margin = FMargin(0.25);
	FSlateBrush Hovered;
	Hovered.TintColor = FSlateColor(FLinearColor(0.783538f, 0.291771f, 0.057805f));
	Hovered.DrawAs = ESlateBrushDrawType::Box;
	Hovered.Margin = FMargin(0.25);
	FButtonStyle InStyle;
	InStyle.SetNormalPadding(FMargin(0.0));
	InStyle.SetPressedPadding(FMargin(0.0));
	InStyle.SetNormal(Normal);
	InStyle.SetHovered(Hovered);
	InStyle.SetPressed(Hovered);
	FButtonStyle InStyleClick;
	InStyleClick.SetNormalPadding(FMargin(0.0));
	InStyleClick.SetPressedPadding(FMargin(0.0));
	InStyleClick.SetNormal(NormalClick);
	InStyleClick.SetHovered(Hovered);
	InStyleClick.SetPressed(Hovered);
	this->InStyle = InStyle;
	this->InStyleClick = InStyleClick;

	TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(this, TEXT("/FactorySkyline/Widget_SelectPanel.Widget_SelectPanel_C"));
	SelectPanel = CreateWidget<UFSSelectPanel>(this, WidgetClass);
	SelectPanel->Init();
	Box2->AddChild(SelectPanel);

	WidgetClass = LoadClass<UUserWidget>(this, TEXT("/FactorySkyline/Widget_DeletePanel.Widget_DeletePanel_C"));
	DeletePanel = CreateWidget<UFSDeletePanel>(this, WidgetClass);
	DeletePanel->Init();
	Box2->AddChild(DeletePanel);

	WidgetClass = LoadClass<UUserWidget>(this, TEXT("/FactorySkyline/Widget_CopyPanel.Widget_CopyPanel_C"));
	CopyPanel = CreateWidget<UFSCopyPanel>(this, WidgetClass);
	CopyPanel->Init();
	Box3->AddChild(CopyPanel);

	WidgetClass = LoadClass<UUserWidget>(this, TEXT("/FactorySkyline/Widget_AdvancedCopyPanel.Widget_AdvancedCopyPanel_C"));
	AdvancedCopyPanel = CreateWidget<UFSAdvancedCopyPanel>(this, WidgetClass);
	AdvancedCopyPanel->Init();
	Box3->AddChild(AdvancedCopyPanel);

	WidgetClass = LoadClass<UUserWidget>(this, TEXT("/FactorySkyline/Widget_EditAreaPanel.Widget_EditAreaPanel_C"));
	EditAreaPanel = CreateWidget<UFSEditAreaPanel>(this, WidgetClass);
	EditAreaPanel->Init();
	Box4->AddChild(EditAreaPanel);

	WidgetClass = LoadClass<UUserWidget>(this, TEXT("/FactorySkyline/Widget_StatusPanel.Widget_StatusPanel_C"));
	StatusPanel = CreateWidget<UFSStatusPanel>(this, WidgetClass);
	StatusPanel->Init();
	Box1->AddChild(StatusPanel);
	
}

void UFSOperatingWidget::Load(UFSDesignMenu* Item)
{
	this->ParentItem = Item;
	this->Design = Item->Design;
	AFSkyline::Get(this)->FSCtrl->LoadDesign(Design);

	SelectPanel->Load(Design);
	DeletePanel->Load(Design);
	CopyPanel->Load(Design);
	AdvancedCopyPanel->Load(Design);
	EditAreaPanel->Load(Design);

	Cast<UCanvasPanelSlot>(Box1->Slot)->SetPosition(FVector2D(0.0f, 0.0f));
	Cast<UCanvasPanelSlot>(Box2->Slot)->SetPosition(FVector2D(0.0f, 0.0f));
	Cast<UCanvasPanelSlot>(Box3->Slot)->SetPosition(FVector2D(0.0f, 0.0f));
	Cast<UCanvasPanelSlot>(Box4->Slot)->SetPosition(FVector2D(0.0f, 0.0f));

	if (Design->BuildableSet.Num() == 0) OnTab2Click();
	else {
		if (TabIndex == 1) StatusPanel->Load(Design);
		else OnTab1Click();
	}
}

void UFSOperatingWidget::Unload()
{
	AFSkyline::Get(this)->FSCtrl->UnloadDesign(true);
}


void UFSOperatingWidget::OnTab1Click()
{
	if (TabIndex == 1) return;
	TabIndex = 1;
	Box1->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Box2->SetVisibility(ESlateVisibility::Hidden);
	Box3->SetVisibility(ESlateVisibility::Hidden);
	Box4->SetVisibility(ESlateVisibility::Hidden);
	Tab1->SetStyle(InStyleClick);
	Tab2->SetStyle(InStyle);
	Tab3->SetStyle(InStyle);
	Tab4->SetStyle(InStyle);
	CurrentBox = Box1;
	StatusPanel->Load(Design);

	AFSkyline* Skyline = AFSkyline::Get(this);

	if (Skyline->FSCtrl->Select != nullptr && Skyline->FSCtrl->Select->Cube != nullptr) {
		if (Skyline->FSCtrl->Select->Cube->IsValidLowLevel() && !Skyline->FSCtrl->Select->Cube->IsPendingKill()) {
			Skyline->FSCtrl->Select->Cube->Destroy();
			Skyline->FSCtrl->Select->Cube = nullptr;
		}
	}

}

void UFSOperatingWidget::OnTab2Click()
{
	if (TabIndex == 2) return;
	TabIndex = 2;
	Box1->SetVisibility(ESlateVisibility::Hidden);
	Box2->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Box3->SetVisibility(ESlateVisibility::Hidden);
	Box4->SetVisibility(ESlateVisibility::Hidden);
	Tab1->SetStyle(InStyle);
	Tab2->SetStyle(InStyleClick);
	Tab3->SetStyle(InStyle);
	Tab4->SetStyle(InStyle);
	CurrentBox = Box2;

	AFSkyline* Skyline = AFSkyline::Get(this);

	if (Skyline->FSCtrl->Select != nullptr && Skyline->FSCtrl->Select->Cube != nullptr) {
		if (Skyline->FSCtrl->Select->Cube->IsValidLowLevel() && !Skyline->FSCtrl->Select->Cube->IsPendingKill()) {
			Skyline->FSCtrl->Select->Cube->Destroy();
			Skyline->FSCtrl->Select->Cube = nullptr;
		}
	}

}

void UFSOperatingWidget::OnTab3Click()
{
	if (TabIndex == 3) return;
	TabIndex = 3;
	Box1->SetVisibility(ESlateVisibility::Hidden);
	Box2->SetVisibility(ESlateVisibility::Hidden);
	Box3->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Box4->SetVisibility(ESlateVisibility::Hidden);
	Tab1->SetStyle(InStyle);
	Tab2->SetStyle(InStyle);
	Tab3->SetStyle(InStyleClick);
	Tab4->SetStyle(InStyle);
	CurrentBox = Box3;

	AFSkyline* Skyline = AFSkyline::Get(this);

	if (Skyline->FSCtrl->Select != nullptr && Skyline->FSCtrl->Select->Cube != nullptr) {
		if (Skyline->FSCtrl->Select->Cube->IsValidLowLevel() && !Skyline->FSCtrl->Select->Cube->IsPendingKill()) {
			Skyline->FSCtrl->Select->Cube->Destroy();
			Skyline->FSCtrl->Select->Cube = nullptr;
		}
	}

}

void UFSOperatingWidget::OnTab4Click()
{
	if (TabIndex == 4) return;
	TabIndex = 4;
	Box1->SetVisibility(ESlateVisibility::Hidden);
	Box2->SetVisibility(ESlateVisibility::Hidden);
	Box3->SetVisibility(ESlateVisibility::Hidden);
	Box4->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Tab1->SetStyle(InStyle);
	Tab2->SetStyle(InStyle);
	Tab3->SetStyle(InStyle);
	Tab4->SetStyle(InStyleClick);
	CurrentBox = Box3;
}

void UFSOperatingWidget::NativeTick(const FGeometry& Geometry, float dt)
{
	Super::NativeTick(Geometry, dt);
	FSMouseWheel(0.0f);
}

FReply UFSOperatingWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FSMouseWheel(InMouseEvent.GetWheelDelta() * 60.0f);
	return FReply::Handled();
}

void UFSOperatingWidget::FSMouseWheel(float DeltaY)
{
	if (!CurrentBox) return;

	UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(CurrentBox->Slot);
	FVector2D Size = CurrentBox->GetCachedGeometry().GetLocalSize();
	float Height = MainPanel->GetCachedGeometry().GetLocalSize().Y;
	float y = Slot->GetPosition().Y + DeltaY;

	if (Size.Y <= Height + 1e-4) y = 0.0f;
	else if (0 < y) y = 0.0f;
	else if (y + Size.Y < Height) y = Height - Size.Y;

	if (FMath::Abs(Slot->GetPosition().Y - y) > 1e-4)
		Slot->SetPosition(FVector2D(0.0f, y));
}

float UFSOperatingWidget::Refresh()
{
	return 0.0f;
}
