// ILikeBanas


#include "FSTowerOperator.h"


FSBuildableType UFSTowerOperator::GetType() const
{
	return FSBuildableType::Building;
}

void UFSTowerOperator::GetSelectConnectList(FSBuildable* Buildable, TArray<TWeakObjectPtr<AFGBuildable>>& List) const
{
}
