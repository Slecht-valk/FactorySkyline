// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "FGOutlineComponent.h"
#include "FGColoredInstanceMeshProxy.h"
#include "Buildables/FGBuildable.h"
//#include "FSDesign.h"
//#include "FSSelectService.h"

//#include "FSController.h"
#include "FGLightweightBuildableSubsystem.h"


#include "AbstractInstanceManager.h"

//#pragma comment(lib, "FactoryGame-AbstractInstance-Win64-Shipping")

//#include "FSBuildable.generated.h"

struct FInstanceData;
struct FInstanceHandle;
//struct FSBuildable;

struct FRuntimeBuildableInstanceData;


struct FSBuildable
{

	// because buildings can have multiple different forms now, either buildings that exist as Afgbuildables or abstracts, we need to determine
	// the type here to better handle specific logic that needs to happen differently for each one
	bool Abstract = false;
	// the handle for this abstract buildable
	FInstanceHandle Handle;
	// the runtime data for this abstract buildable
	//FRuntimeBuildableInstanceData* RuntimeData = new FRuntimeBuildableInstanceData();
	FRuntimeBuildableInstanceData* RuntimeData = nullptr;
	//
	TSubclassOf< AFGBuildable > BuildableClass = nullptr;

	FTransform Transform;

	// the data stored above is needed for the abstracts only, for buildables that also have instance data we can just store the reference here instead
	AFGBuildable* Buildable = nullptr;

	friend bool Operator==(const FSBuildable& Left, const FSBuildable& Right);

	/*
	bool operator==(const FSBuildable& Other) const {
		if (BuildableClass == nullptr) {
			return Buildable == Other.Buildable;
		}
		else {

			UHierarchicalInstancedStaticMeshComponent* hismc = Handle.GetInstanceComponent();
			UHierarchicalInstancedStaticMeshComponent* hismcOther = Other.Handle.GetInstanceComponent();

			// Check if they share the same mesh component.
			if (hismc == hismcOther) {
				return Handle.HandleID == Other.Handle.HandleID;
			}
			else {
				return false;
			}

			//UHierarchicalInstancedStaticMeshComponent* hismc = Handle.GetInstanceComponent();
			//UHierarchicalInstancedStaticMeshComponent* hismcOther = Other.Handle.GetInstanceComponent();
			//return hismc == hismcOther && Transform.Equals(Other.Transform, 0.0001f) && Handle.HandleID == Other.Handle.HandleID && BuildableClass == Other.BuildableClass;
		}

		return false;

		
		//if (Buildable) {
			//return Buildable == Other.Buildable;
		//}
		//else {
			//return Abstract == Other.Abstract && (Handle.HandleID == Other.Handle.HandleID) && RuntimeData == Other.RuntimeData && BuildableClass == Other.BuildableClass;
		//}
		
		//return Abstract == Other.Abstract && (Handle.HandleID == Other.Handle.HandleID) && RuntimeData == Other.RuntimeData && BuildableClass == Other.BuildableClass;
	}
	*/


	/*
	bool operator!=(const FSBuildable& Other) const {
		if (BuildableClass == nullptr) {
		} else {

			UHierarchicalInstancedStaticMeshComponent* hismc = Handle.GetInstanceComponent();
			UHierarchicalInstancedStaticMeshComponent* hismcOther = Other.Handle.GetInstanceComponent();

			// Check if they share the same mesh component.
			if (hismc == hismcOther) {
				return Handle.HandleID != Other.Handle.HandleID;
			}
			else {
				return false;
			}

		}
	}
	*/
	

};

// Define operator== outside the struct
bool Operator==(const FSBuildable& Left, const FSBuildable& Right) {
	if (Left.BuildableClass == nullptr) {
		return Left.Buildable == Right.Buildable;
	}
	else {
		UHierarchicalInstancedStaticMeshComponent* hismc = Left.Handle.GetInstanceComponent();
		UHierarchicalInstancedStaticMeshComponent* hismcOther = Right.Handle.GetInstanceComponent();
		return hismc == hismcOther
			&& Left.Transform.Equals(Right.Transform, 0.0001f)
			&& Left.Handle.HandleID == Right.Handle.HandleID
			&& Left.BuildableClass == Right.BuildableClass;
	}
}


// Define GetTypeHash for FSBuildable
FORCEINLINE uint32 GetTypeHash(const FSBuildable& Buildable)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Buildable.Abstract));
	Hash = HashCombine(Hash, GetTypeHash(Buildable.Handle.HandleID)); // Assuming HandleID is hashable
	Hash = HashCombine(Hash, GetTypeHash(Buildable.RuntimeData));
	Hash = HashCombine(Hash, GetTypeHash(Buildable.BuildableClass));
	Hash = HashCombine(Hash, GetTypeHash(Buildable.Transform));
	Hash = HashCombine(Hash, GetTypeHash(Buildable.Buildable));
	// Consider whether to include RuntimeData in the hash

	return Hash;
}
