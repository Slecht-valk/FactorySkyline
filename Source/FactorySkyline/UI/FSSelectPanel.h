// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "FactorySkyline/UI/FSFoldWidgetBase.h"
#include "FactorySkyline/FSDesign.h"
#include "Blueprint/UserWidget.h"
#include "FSSelectPanel.generated.h"

/**
 * 
 */
UCLASS()
class FACTORYSKYLINE_API UFSSelectPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	UFSSelectPanel(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void onCheckBoxChanged(bool IsChecked);

	UFUNCTION()
	void onStartSelect();

	void Init();
	void Load(UFSDesign* Design);

	void Fold();
	void Expand();

	UFUNCTION()
	void onCheckBoxAddChanged(bool IsChecked);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* SelectButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Readme;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UFSFoldWidgetBase* FoldPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCheckBox* CheckBoxAdd;

	UFSDesign* Design = nullptr;
	//FButtonStyle ButtonStyle;
};
