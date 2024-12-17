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
	FRuntimeBuildableInstanceData RuntimeData;
	//
	TSubclassOf< AFGBuildable > BuildableClass = nullptr;

	FTransform Transform;

	// the data stored above is needed for the abstracts only, for buildables that also have instance data we can just store the reference here instead
	AFGBuildable* Buildable = nullptr;

	friend uint32 GetTypeHash(const FSBuildable& Buildable);

	// Custom GetTransform method to return Transform field
	const FTransform& GetTransform() const
	{
		return Transform;
	}

	bool operator==(const FSBuildable& Other) const {

		if (GetTypeHash(*this) == GetTypeHash(Other)) {
			return true;  // Quickly rule out different structs
		}
		return false;

		if (Buildable|| Other.Buildable) {
			return Buildable == Other.Buildable;
		}
		else {

			if (BuildableClass != Other.BuildableClass) {
				return false;
			}
			else {

				UHierarchicalInstancedStaticMeshComponent* hismc = Handle.GetInstanceComponent();
				UHierarchicalInstancedStaticMeshComponent* hismcOther = Other.Handle.GetInstanceComponent();

				// Check if they share the same mesh component.
				if (hismc == hismcOther) {
					// If they do, ensure the HandleIDs don't match as well.
					if (Handle.HandleID == Other.Handle.HandleID) {
						return true;
					}
					else {
						return false;
					}
				}
				else {
					// Different mesh components, so they are distinct by default.
					return false;
				}
			}
		}
	}

	bool operator!=(const FSBuildable& Other) const {

		if (GetTypeHash(*this) != GetTypeHash(Other)) {
			return true;  // Quickly rule out different structs
		}
		return false;


		if (BuildableClass == nullptr && Other.BuildableClass == nullptr) {
		//if (Buildable && Other.Buildable) {
			return Buildable != Other.Buildable;
		}
		else {

			if (BuildableClass != Other.BuildableClass) {
				return true;
			}
			else {

				UHierarchicalInstancedStaticMeshComponent* hismc = Handle.GetInstanceComponent();
				UHierarchicalInstancedStaticMeshComponent* hismcOther = Other.Handle.GetInstanceComponent();

				// Check if they share the same mesh component.
				if (hismc == hismcOther) {
					// If they do, ensure the HandleIDs don't match as well.
					if (Handle.HandleID != Other.Handle.HandleID) {
						return true;
					}
					else {
						return false;
					}
				} else {
					// Different mesh components, so they are distinct by default.
					return true;
				}
			}
		}

		/*
		if (Buildable != Other.Buildable && BuildableClass != Other.BuildableClass) {
			return true;
		}
		
		if (Buildable == Other.Buildable && BuildableClass == Other.BuildableClass) {
			return false;
		}

		// this is a struct thats empty, those nothing is equal to it;
		if (this->Buildable == nullptr && BuildableClass == nullptr) {
			return true;
		}

		if (BuildableClass == nullptr) {
			return Buildable != Other.Buildable;
		} else {

			UHierarchicalInstancedStaticMeshComponent* hismc = Handle.GetInstanceComponent();
			UHierarchicalInstancedStaticMeshComponent* hismcOther = Other.Handle.GetInstanceComponent();

			// Check if they share the same mesh component.
			if (hismc == hismcOther) {
				// If they do, ensure the HandleIDs match as well.
				return Handle.HandleID != Other.Handle.HandleID;
			}
			else {
				// Different mesh components, so they are distinct by default.
				return true;
			}

		}
		*/

	}
};


// Define GetTypeHash for FSBuildable
FORCEINLINE uint32 GetTypeHash(const FSBuildable& Buildable)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Buildable.Abstract));
	Hash = HashCombine(Hash, GetTypeHash(Buildable.Handle.HandleID)); // Assuming HandleID is hashable
	//Hash = HashCombine(Hash, GetTypeHash(Buildable.RuntimeData));
	Hash = HashCombine(Hash, GetTypeHash(Buildable.BuildableClass));
	// these can have tiny differences even for the same exact instance which can lead to differences in hash values so not stable to use
	//Hash = HashCombine(Hash, GetTypeHash(Buildable.Transform));
	Hash = HashCombine(Hash, GetTypeHash(Buildable.Handle.GetInstanceComponent()));
	Hash = HashCombine(Hash, GetTypeHash(Buildable.Buildable));
	// Consider whether to include RuntimeData in the hash

	return Hash;
}

struct FSBuildableTest
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

	// Custom constructor
	FSBuildableTest(AFGBuildable* InBuildable)
		: Buildable(InBuildable)
	{
		Buildable = InBuildable;
	}

	// Default constructor
	FSBuildableTest() = default;

	bool operator==(const FSBuildableTest& Other) const {

		if (Buildable && Other.Buildable) {
			return Buildable == Other.Buildable;
		}
		else {

			if (BuildableClass != Other.BuildableClass) {
				return false;
			}
			else {

				UHierarchicalInstancedStaticMeshComponent* hismc = Handle.GetInstanceComponent();
				UHierarchicalInstancedStaticMeshComponent* hismcOther = Other.Handle.GetInstanceComponent();

				// Check if they share the same mesh component.
				if (hismc == hismcOther) {
					// If they do, ensure the HandleIDs don't match as well.
					if (Handle.HandleID == Other.Handle.HandleID) {
						return true;
					}
					else {
						return false;
					}
				}
				else {
					// Different mesh components, so they are distinct by default.
					return false;
				}
			}
		}
	}

	bool operator!=(const FSBuildableTest& Other) const {


		if (Buildable && Other.Buildable) {
			return Buildable != Other.Buildable;
		}
		else {

			if (BuildableClass != Other.BuildableClass) {
				return true;
			}
			else {

				UHierarchicalInstancedStaticMeshComponent* hismc = Handle.GetInstanceComponent();
				UHierarchicalInstancedStaticMeshComponent* hismcOther = Other.Handle.GetInstanceComponent();

				// Check if they share the same mesh component.
				if (hismc == hismcOther) {
					// If they do, ensure the HandleIDs don't match as well.
					if (Handle.HandleID != Other.Handle.HandleID) {
						return true;
					}
					else {
						return false;
					}
				}
				else {
					// Different mesh components, so they are distinct by default.
					return true;
				}
			}
		}

		/*
		if (Buildable != Other.Buildable && BuildableClass != Other.BuildableClass) {
			return true;
		}

		if (Buildable == Other.Buildable && BuildableClass == Other.BuildableClass) {
			return false;
		}

		// this is a struct thats empty, those nothing is equal to it;
		if (this->Buildable == nullptr && BuildableClass == nullptr) {
			return true;
		}

		if (BuildableClass == nullptr) {
			return Buildable != Other.Buildable;
		} else {

			UHierarchicalInstancedStaticMeshComponent* hismc = Handle.GetInstanceComponent();
			UHierarchicalInstancedStaticMeshComponent* hismcOther = Other.Handle.GetInstanceComponent();

			// Check if they share the same mesh component.
			if (hismc == hismcOther) {
				// If they do, ensure the HandleIDs match as well.
				return Handle.HandleID != Other.Handle.HandleID;
			}
			else {
				// Different mesh components, so they are distinct by default.
				return true;
			}

		}
		*/

	}
};