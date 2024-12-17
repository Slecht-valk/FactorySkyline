// ILikeBanas


#include "FSWireOperator.h"
#include "Buildables/FGBuildableWire.h"
#include "Hologram/FGWireHologram.h"
#include "FGCircuitConnectionComponent.h"

AFGBuildable* UFSWireOperator::CreateCopy(const FSTransformOperator& TransformOperator)
{
	//SML::Logging::info(TEXT("UFSWireOperator::CreateCopy"));

	AFGBuildableWire* SourceWire = nullptr;

	if (Source.Buildable) {
		SourceWire = Cast<AFGBuildableWire>(Source.Buildable);
	}

	if (!SourceWire->GetConnection(0)) return nullptr;
	if (!SourceWire->GetConnection(1)) return nullptr;

	UFGCircuitConnectionComponent* TargetConnection0 = this->ConnectionMapping<UFGCircuitConnectionComponent>(SourceWire->GetConnection(0));
	UFGCircuitConnectionComponent* TargetConnection1 = this->ConnectionMapping<UFGCircuitConnectionComponent>(SourceWire->GetConnection(1));

	if (!TargetConnection0 && !TargetConnection1) return nullptr;

	AActor* actor0 = nullptr;
	AActor* actor1 = nullptr;

	TArray<AActor*> actors;

	if (!TargetConnection0 || !TargetConnection1) {
		AFGBuildable* SourceExternal = Cast<AFGBuildable>(SourceWire->GetConnection(TargetConnection0 ? 1 : 0)->GetAttachmentRootActor());
		if (SourceExternal) {
			FVector TargetLocation = TransformOperator.Transform(SourceExternal->GetTransform()).GetLocation();
			float QueryDist = 50.0f;
			//AFGBuildable* TargetExternal = BuildableService->StaticTree.QueryNearest(TargetLocation, QueryDist);

			TArray<FOverlapResult> Result;
			FCollisionShape Shape;
			Shape.SetSphere(50.0f);
			World->OverlapMultiByChannel(Result, TargetLocation, FRotator::ZeroRotator.Quaternion(), ECollisionChannel::ECC_Visibility, Shape);

			//AActor* actor0 = nullptr;
			//AActor* actor1 = nullptr;

			for (FOverlapResult& Res : Result) {
				if (Res.GetActor()->GetClass() == SourceExternal->GetClass()) {
					UFGCircuitConnectionComponent* Connection = Cast<UFGCircuitConnectionComponent>(Res.GetActor()->GetComponentByClass(UFGCircuitConnectionComponent::StaticClass()));
					if (Connection->GetNumFreeConnections()) {
						if (!TargetConnection0) {
							actor0 = Res.GetActor();
							TargetConnection0 = Connection;
						}
						if (!TargetConnection1) {
							actor1 = Res.GetActor();
							TargetConnection1 = Connection;
						}
						break;
					}
				}
			}
		}
	}

	if (!TargetConnection0 || !TargetConnection1) return nullptr;

	//FTransform Transform = TransformOperator.Transform(Source->GetTransform());

	FTransform Transform;

	if (Source.Buildable) {
		Transform = TransformOperator.Transform(Source.Buildable->GetTransform());
	}

	AFGBuildable* Buildable = nullptr;

	if (Source.Buildable) {
		Buildable = BuildableSubsystem->BeginSpawnBuildable(Source.Buildable->GetClass(), Transform);
	}
	TSubclassOf<UFGRecipe> Recipe;
	if (Source.Buildable) {
		Recipe = SplineHologramFactory->GetRecipeFromClass(Source.Buildable->GetClass());
		if (!Recipe) Recipe = Source.Buildable->GetBuiltWithRecipe();
	}
	if (!Recipe) return nullptr;

	Buildable->SetBuiltWithRecipe(Recipe);

	AFGBuildableWire* Wire = Cast<AFGBuildableWire>(Buildable);
	Wire->Connect(TargetConnection0, TargetConnection1);

	actors.Add(actor0);
	actors.Add(actor1);

	if (Source.Buildable) {
		Buildable->SetCustomizationData_Implementation(Source.Buildable->GetCustomizationData_Implementation());
	}
	Buildable->FinishSpawning(Transform);

	return Buildable;
}

FSBuildableType UFSWireOperator::GetType() const
{
	return FSBuildableType::Power;
}

void UFSWireOperator::GetSelectConnectList(FSBuildable* Buildable, TArray<TWeakObjectPtr<AFGBuildable>>& List) const
{
	if (Buildable->Buildable) {
		AFGBuildableWire* Wire = Cast<AFGBuildableWire>(Buildable->Buildable);
		if (!Wire) return;

		for (int i = 0; i < 2; i++) {
			UFGCircuitConnectionComponent* ConnectionComponent = (UFGCircuitConnectionComponent*)Wire->GetConnection(i);
			if (!ConnectionComponent) continue;

			TArray<UFGCircuitConnectionComponent*> Connections;
			ConnectionComponent->GetConnections(Connections);

			for (UFGCircuitConnectionComponent* TargetConnection : Connections) {
				AFGBuildable* Connection = Cast<AFGBuildable>(TargetConnection->GetAttachmentRootActor());
				if (Connection) List.Add(Connection);
			}
		}
	}
}