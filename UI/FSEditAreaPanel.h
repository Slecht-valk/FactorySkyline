// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "FactorySkyline/UI/FSFoldWidgetBase.h"
#include "FactorySkyline/FSDesign.h"
#include "Blueprint/UserWidget.h"
#include "FSEditAreaPanel.generated.h"

/**
 *
 */
UCLASS()
class FACTORYSKYLINE_API UFSEditAreaPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	UFSEditAreaPanel(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void onCheckBoxChanged(bool IsChecked);

	UFUNCTION()
	void onFullPreviewChanged(bool IsChecked);

	UFUNCTION()
	void onSetItem();

	UFUNCTION()
	void onStartCopy();

	UFUNCTION()
	void onChangeAreaBox();

	void Init();
	void Load(UFSDesign* Design);

	void Fold();
	void Expand();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* SetItemButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* StartCopyButton;


	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	//class UFSFoldWidgetBase* FoldPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* ChangeSelectionBoxButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UEditableText* Count;

	UFSDesign* Design = nullptr;
};
