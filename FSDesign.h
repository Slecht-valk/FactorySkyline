// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Buildables/FGBuildable.h"
#include "Components/TextBlock.h"

//#include "FSBuildable.h"

struct FSBuildable;

#include "FSDesign.generated.h"

/**
 * 
 */
UCLASS()
class FACTORYSKYLINE_API UFSDesign : public UObject, public IFGSaveInterface
{
	GENERATED_BODY()
public:

	virtual bool ShouldSave_Implementation() const override;
	void PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion) override;
	void PostSaveGame_Implementation(int32 saveVersion, int32 gameVersion) override;
	void PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion) override;

	bool IsElementSelected(FSBuildable* Buildable);
	void AddElement(FSBuildable* Buildable);
	void RemoveElement(FSBuildable* Buildable);
	void SetElementMark(FSBuildable* Buildable, bool MarkParam);

	void ClearAll();
	void DismantleAll();

	void RecheckNullptr();

	TSet<FSBuildable*> BuildableSet;
	TMap<FSBuildable*, int> BuildableMark;
	FSBuildable* Anchor;

	//TArray<AFGBuildable*> BuildableArray;

	UPROPERTY(SaveGame)
	TSet<AFGBuildable*> Set;

	UPROPERTY(SaveGame)
	TMap<AFGBuildable*, int> Mark;

	UPROPERTY(SaveGame)
	AFGBuildable* AnchorSave = nullptr;
	
	UTextBlock* SetItemFeedback = nullptr;

	bool AddConstructedBuildable = false;
};
