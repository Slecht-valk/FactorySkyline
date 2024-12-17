// ILikeBanas


#include "FSDesign.h"
#include "FSkyline.h"
#include "FSController.h"
#include "Buildables/FGBuildable.h"

#include "FGBuildablePipelineFlowIndicator.h"

bool UFSDesign::ShouldSave_Implementation() const 
{
	AFSkyline* FSkyline = AFSkyline::Get(this->GetWorld());
	return FSkyline && FSkyline->FSCtrl && FSkyline->FSCtrl->Etc ? !FSkyline->FSCtrl->Etc->GetBool("DontSaveAnything") : false;
}

void UFSDesign::PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion)
{
	/*
	AnchorSave = Anchor.Get();
	Set.Empty();
	for (TWeakObjectPtr<AFGBuildable>& Ptr : BuildableSet) {
		if (Ptr.Get()) Set.Add(Ptr.Get());
	}
	Mark.Empty();
	for (TPair<TWeakObjectPtr<AFGBuildable>,int>& Pair : BuildableMark) {
		if (Pair.Key.Get()) Mark.Add(Pair.Key.Get(), Pair.Value);
	}
	*/
}

void UFSDesign::PostSaveGame_Implementation(int32 saveVersion, int32 gameVersion)
{
	AnchorSave = nullptr;
	Set.Empty();
	Mark.Empty();
}

void UFSDesign::PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion)
{
	/*
	Anchor = AnchorSave;
	BuildableSet.Empty();
	BuildableMark.Empty();
	for (AFGBuildable*& Ptr : Set) {
		BuildableSet.Add(Ptr);
	}
	for (TPair<AFGBuildable*, int>& Pair : Mark) {
		BuildableMark.Add(Pair.Key, Pair.Value);
	}
	AnchorSave = nullptr;
	Set.Empty();
	Mark.Empty();
	*/
}

bool UFSDesign::IsElementSelected(FSBuildable Buildable)
{

	if (!&Buildable) return false;

	// If `BuildableSet` contains FSBuildable pointers (TSet<FSBuildable*>)
	FSBuildable* Result1 = BuildableSet.Find(Buildable);
	return Result1 != nullptr;

	// Alternatively, if BuildableSet contains FSBuildable structs (TSet<FSBuildable>)
	return BuildableSet.Contains(Buildable);

	/*
	if (!Buildable) return false;

	FSBuildable** Result1 = BuildableSet.Find(Buildable);

	if (Result1) {
	} else {
		return false;
	}

	FSBuildable* Result2 = *Result1;

	if (Result2) {
		return true;
	}else {
		return false;
	}
	*/

	//return Result && Result->Get();
}

void UFSDesign::AddElement(FSBuildable Buildable)
{
	BuildableSet.Add(Buildable);
	//BuildableArray.Add(Buildable);
}

void UFSDesign::RemoveElement(FSBuildable Buildable)
{
	BuildableSet.Remove(Buildable);
	//BuildableArray.Remove(Buildable);

}

void UFSDesign::SetElementMark(FSBuildable Buildable, bool MarkParam)
{
	//BuildableMark[Buildable] = Mark;
}

void UFSDesign::ClearAll()
{
	this->BuildableSet.Empty();
	this->BuildableMark.Empty();
	FSBuildable Buildable;
	this->Anchor = Buildable;
}

void UFSDesign::DismantleAll()
{
	// TODO WE NEED TO REWORK THIS
	/*
	AFSkyline* FSkyline = AFSkyline::Get(this->GetWorld());
	TArray<AFGBuildable*> List;
	for (TWeakObjectPtr<AFGBuildable> Buildable : BuildableSet) if (Buildable.Get()) {
		FSkyline->FSCtrl->Inventory.AddResourceCheckRecipe(Buildable.Get());

		//Buildable.Get()->SetHiddenIngameAndHideInstancedMeshes(true);

		for (UActorComponent* ActorComponent : Buildable.Get()->GetComponents()) {
			ActorComponent->UnregisterComponent();
		}
		List.Add(Buildable.Get());
	}
	for (int i = 0; i < List.Num(); i++) {
		List[i]->TurnOffAndDestroy();
	}

	ClearAll();
	*/

	AFSkyline* FSkyline = AFSkyline::Get(this->GetWorld());
	TArray<AFGBuildable*> List;
	TArray<AActor*> List2;

	// Assuming the following variables are already defined and initialized appropriately:
	//AActor* ActorToDismantle; /* Pointer to the actor you want to dismantle */;
	TArray<AActor*> CouldNotDismantle;          // Array to collect actors that could not be dismantled
	TArray<FInventoryStack> DismantleRefunds;   // Array to collect dismantle refunds
	bool bNoBuildCostEnabled = true;            // Flag indicating whether to ignore build costs

	for (FSBuildable Buildable : BuildableSet) {
		//if (Buildable.Get()) {
			FSkyline->FSCtrl->Inventory.AddResourceCheckRecipe(Buildable);

			//Buildable.Get()->SetHiddenIngameAndHideInstancedMeshes(true);

			/*
			for (UActorComponent* ActorComponent : Buildable.Get()->GetComponents()) {
				ActorComponent->UnregisterComponent();
			}
			*/

			if (Buildable.Buildable) {
				//List.Add(Buildable.Buildable);
				//List2.Add(Buildable.Buildable);

				if (Buildable.Buildable->GetClass()->IsChildOf<AFGBuildablePipeBase>()) {
					AFGBuildablePipeline* pipeBuildable = Cast<AFGBuildablePipeline>(Buildable.Buildable);

					// because pipes are more of a special remove case as they can also have a seperate buildable attached to them that needs to be removed as well otherwise
					// it'll linger in the world
					if (pipeBuildable) {
						AFGBuildablePipelineFlowIndicator* Indicator = pipeBuildable->GetFlowIndicator();
						if (Indicator) {
							//IFGDismantleInterface::Execute_Dismantle(Indicator);
							Indicator->TurnOffAndDestroy();
						}
					}

				}

				for (UActorComponent* ActorComponent : Buildable.Buildable->GetComponents()) {
					ActorComponent->UnregisterComponent();
				}

				Buildable.Buildable->TurnOffAndDestroy();

			}
			else {

				FLightweightBuildableInstanceRef buildableRef;
				AFGLightweightBuildableSubsystem::ResolveLightweightInstance(Buildable.Handle, buildableRef);
				buildableRef.Remove();

			}
		//}
	}

	/*
	UFGBuildGunStateDismantle* dismantleState = FSkyline->FSCtrl->FGBuildGun->mDismantleStateClass.GetDefaultObject();

	//FSkylin->FSCtrl->FGBuildGun->GotoDismantleState();
	FSkyline->FSCtrl->GetPlayer()->SetOverrideEquipment(FSkyline->FSCtrl->FGBuildGun);
	AFGEquipment* Equipment = FSkyline->FSCtrl->GetPlayer()->GetEquipmentInSlot(EEquipmentSlot::ES_ARMS);
	FSkyline->FSCtrl->GetPlayer()->EquipEquipment(FSkyline->FSCtrl->FGBuildGun);
	FSkyline->FSCtrl->FGBuildGun->Equip(FSkyline->FSCtrl->GetPlayer());
	FSkyline->FSCtrl->FGBuildGun->GotoStateInternal(EBuildGunState::BGS_DISMANTLE);
	//dismantleState->BeginState_Implementation()
	*/

	//AFGBuildableSubsystem* Buildablesubsystem;
	//Buildablesubsystem = AFGBuildableSubsystem::Get(FSkyline->World);

	for (int i = 0; i < List.Num(); i++) {

		//List[i]->SetMarkedForDismantle();
		//dismantleState->AddPendingDismantleActor(List[i]);
		//List[i]->Dismantle();
		//List[i]->execDismantle();
		//List[i]->Execute_Dismantle(this);

		//IFGDismantleInterface::Execute_Dismantle(List[i]);

		//dismantleState->AddPendingDismantleActor(List[i]);

		//ActorToDismantle = List[i];
		//IFGDismantleInterface::Execute_Dismantle(List[i]);

		//dismantleState->Internal_DismantleActor(ActorToDismantle, CouldNotDismantle, DismantleRefunds, bNoBuildCostEnabled);

		//dismantleState->PrimaryFire_Implementation();

		/*
		for (UActorComponent* ActorComponent : List[i]->GetComponents()) {
			ActorComponent->UnregisterComponent();
		}
		*/

		//List[i]->TurnOffAndDestroy();

		//Buildablesubsystem->RemoveBuildable(List[i]);

		/*
		Buildablesubsystem->mFactoryBuildingGroupsDirty = true;
		Buildablesubsystem->mConveyorBucketGroupsDirty = true;
		Buildablesubsystem->mConveyorAttachmentGroupsDirty = true;
		Buildablesubsystem->mClientConveyorChainGroupsDirty = true;
		*/

		bool chainactor = false;

		if (List[i]->GetClass()->IsChildOf<AFGConveyorChainActor>() || List[i]->GetClass()->IsChildOf<AFGBuildableConveyorBase>()) {
			chainactor = true;
			AFGBuildableConveyorBelt* BeltBuildable = Cast<AFGBuildableConveyorBelt>(List[i]);
			if (BeltBuildable) {
				/*
				AFGConveyorChainActor* chainActor = BeltBuildable->GetConveyorChainActor();
				BeltBuildable->SetConveyorChainActor(nullptr);
				UFGFactoryConnectionComponent* connection0 = BeltBuildable->GetConnection0();
				UFGFactoryConnectionComponent* connection1 = BeltBuildable->GetConnection1();
				BeltBuildable->mConnection0 = nullptr;
				BeltBuildable->mConnection1 = nullptr;
				*/

				//IFGDismantleInterface::Execute_Dismantle(BeltBuildable);

				//chainActor->SanitizeItemsFromLoad();
				//chainActor->RemoveClientAvailableConveyor(BeltBuildable);
				//Buildablesubsystem->RemoveConveyorChainActor(chainActor);
				//chainActor->Destroy();
				//BeltBuildable->SetConveyorChainActor(nullptr);
				//IFGDismantleInterface::Execute_Dismantle(BeltBuildable);
			}
		}
		else {
			//List[i]->Destroy();
			//continue;
			//IFGDismantleInterface::Execute_Dismantle(List[i]);
		}

	}

	//dismantleState->SetMultiDismantleState(true);

	//TArray< AActor* > out_couldNotDismantle;
	//TArray<FInventoryStack> out_dismantleRefunds;


	//dismantleState->OnStartDismantle();

	
	//AFGBuildableSubsystem* Buildablesubsystem;
	//Buildablesubsystem = AFGBuildableSubsystem::Get(FSkyline->World);
	for (int i = 0; i < List.Num(); i++) {

		if (List[i]->GetClass()->IsChildOf<AFGBuildableConveyorBase>()) {
			AFGBuildablePipeline* pipeBuildable = Cast<AFGBuildablePipeline>(List[i]);

			// because pipes are more of a special remove case as they can also have a seperate buildable attached to them that needs to be removed as well otherwise
			// it'll linger in the world
			if (pipeBuildable) {
				AFGBuildablePipelineFlowIndicator* Indicator = pipeBuildable->GetFlowIndicator();
				if (Indicator) {
					IFGDismantleInterface::Execute_Dismantle(Indicator);
					//Indicator->TurnOffAndDestroy();
				}
			}

		}

		//Buildablesubsystem->RemoveBuildable(List[i]);

		for (UActorComponent* ActorComponent : List[i]->GetComponents()) {
			ActorComponent->UnregisterComponent();
		}

		List[i]->TurnOffAndDestroy();
	}
	

	ClearAll();


}

void UFSDesign::RecheckNullptr()
{
	// TODO WE NEED TO REWORK THIS
	/*
	TSet< TWeakObjectPtr<AFGBuildable> > SetVar;
	TMap< TWeakObjectPtr<AFGBuildable>, int> MarkVar;

	for (TWeakObjectPtr<AFGBuildable>& Ptr : this->BuildableSet)
		if (Ptr.Get()) SetVar.Add(Ptr);
	for (TPair<TWeakObjectPtr<AFGBuildable>, int>& Pair : this->BuildableMark)
		if (Pair.Key.Get()) MarkVar.Add(Pair);

	this->BuildableSet = SetVar;
	this->BuildableMark = MarkVar;
	*/
}