// ILikeBanas


#include "FSTradingPostOperator.h"
#include "Buildables/FGBuildableTradingPost.h"
#include "FGFactoryLegsComponent.h"


FSBuildableType UFSTradingPostOperator::GetType() const
{
	return FSBuildableType::Building;
}
void UFSTradingPostOperator::GetSelectConnectList(FSBuildable* Buildable, TArray<TWeakObjectPtr<AFGBuildable>>& List) const
{
}