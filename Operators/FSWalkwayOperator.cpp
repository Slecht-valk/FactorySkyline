// ILikeBanas


#include "FSWalkwayOperator.h"
#include "Buildables/FGBuildableFactoryBuilding.h"
#include "Buildables/FGBuildableWalkway.h"


void UFSWalkwayOperator::GetSelectConnectList(FSBuildable* Buildable, TArray<TWeakObjectPtr<AFGBuildable>>& List) const
{
	if (Buildable->Buildable) {
		AFGBuildableWalkway* Walkway = Cast<AFGBuildableWalkway>(Buildable->Buildable);
		if (Walkway) {
			TArray<FOverlapResult> Result;
			FCollisionShape Shape;
			if (Walkway->mElevation > 10.0f) {
				FTransform Transform = Walkway->GetTransform();
				FVector Loc = Transform.GetLocation();

				Shape.SetBox(FVector3f(Walkway->mSize / 2.0, Walkway->mSize / 2.0 - 10.0f, Walkway->mElevation / 2.0));
				World->OverlapMultiByChannel(Result, FVector(Loc.X + 50.0f, Loc.Y, Loc.Z + Walkway->mElevation / 2.0 + 50.0f), Transform.GetRotation(), ECollisionChannel::ECC_Visibility, Shape);
				World->OverlapMultiByChannel(Result, FVector(Loc.X - 50.0f, Loc.Y, Loc.Z + Walkway->mElevation / 2.0 - 50.0f), Transform.GetRotation(), ECollisionChannel::ECC_Visibility, Shape);

				Shape.SetBox(FVector3f(Walkway->mSize / 2.0 - 10.0f, Walkway->mSize / 2.0 + 50.0f, Walkway->mElevation / 2.0 - 10.0f));
				World->OverlapMultiByChannel(Result, FVector(Loc.X, Loc.Y, Loc.Z + Walkway->mElevation / 2.0), Transform.GetRotation(), ECollisionChannel::ECC_Visibility, Shape);
			}
			else {
				FTransform Transform = Walkway->GetTransform();

				Shape.SetBox(FVector3f(Walkway->mSize / 2.0 + 50.0f, Walkway->mSize / 2.0 - 10.0f, 50.0f));
				World->OverlapMultiByChannel(Result, Transform.GetLocation(), Transform.GetRotation(), ECollisionChannel::ECC_Visibility, Shape);

				Shape.SetBox(FVector3f(Walkway->mSize / 2.0 - 10.0f, Walkway->mSize / 2.0 + 50.0f, 50.0f));
				World->OverlapMultiByChannel(Result, Transform.GetLocation(), Transform.GetRotation(), ECollisionChannel::ECC_Visibility, Shape);
			}

			for (FOverlapResult& Res : Result) {
				AFGBuildable* Buildable = Cast<AFGBuildable>(Res.GetActor());
				if (Buildable) List.Add(Buildable);
			}
		}
	}
}

