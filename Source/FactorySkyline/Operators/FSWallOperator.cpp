// ILikeBanas


#include "FSWallOperator.h"
#include "Buildables/FGBuildableFactoryBuilding.h"
#include "Buildables/FGBuildableWall.h"


void UFSWallOperator::GetSelectConnectList(FSBuildable* Buildable, TArray<TWeakObjectPtr<AFGBuildable>>& List) const
{
	if (Buildable->Buildable) {
		AFGBuildableWall* Wall = Cast<AFGBuildableWall>(Buildable->Buildable);
		if (Wall) {
			TArray<FOverlapResult> Result;
			FCollisionShape Shape;
			FTransform Transform = Wall->GetTransform();
			FVector Loc = Transform.GetLocation();

			Shape.SetBox(FVector3f(50.0f, Wall->mWidth / 2.0 + 50.0f, Wall->mHeight / 2.0 - 10.0f));
			World->OverlapMultiByChannel(Result, FVector(Loc.X, Loc.Y, Loc.Z + Wall->mHeight / 2.0), Transform.GetRotation(), ECollisionChannel::ECC_Visibility, Shape);

			Shape.SetBox(FVector3f(50.0f, Wall->mWidth / 2.0 - 10.0f, Wall->mHeight / 2.0 + 50.0f));
			World->OverlapMultiByChannel(Result, FVector(Loc.X, Loc.Y, Loc.Z + Wall->mHeight / 2.0), Transform.GetRotation(), ECollisionChannel::ECC_Visibility, Shape);

			for (FOverlapResult& Res : Result) {
				AFGBuildable* Buildable = Cast<AFGBuildable>(Res.GetActor());
				if (Buildable) List.Add(Buildable);
			}
		}
	}
}

