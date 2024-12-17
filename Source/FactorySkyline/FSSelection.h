// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "FGOutlineComponent.h"
#include "FGColoredInstanceMeshProxy.h"
#include "Buildables/FGBuildable.h"
#include "FSDesign.h"
#include "FSSelectService.h"

#include "InstanceData.h"
#include "HologramHelper.h"
#include "MyActor.h"

//#include "FSController.h"
#include "FGLightweightBuildableSubsystem.h"


#include "AbstractInstanceManager.h"

#include "FSBuildable.h"

//#pragma comment(lib, "FactoryGame-AbstractInstance-Win64-Shipping")

#include "FSSelection.generated.h"

struct FInstanceData;
struct FInstanceHandle;
//struct FSBuildable;

struct FRuntimeBuildableInstanceData;


/*
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

	bool operator==(const FSBuildable& Other) const {

		if (BuildableClass == nullptr) {
			return Buildable == Other.Buildable;
		}
		else {
			UHierarchicalInstancedStaticMeshComponent* hismc = Handle.GetInstanceComponent();
			UHierarchicalInstancedStaticMeshComponent* hismcOther = Other.Handle.GetInstanceComponent();
			return hismc == hismcOther && Transform.Equals(Other.Transform, 0.0001f) && Handle.HandleID == Other.Handle.HandleID && BuildableClass == Other.BuildableClass;
		}

		return false;
	}

};


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
*/

struct FMaterialPair
{
	//GENERATED_BODY()

	UPROPERTY()
	UMaterialInstanceDynamic* SelectMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* FocusMaterial;

	UPROPERTY()
	AHologramHelper* SelectActor = nullptr;

	UHierarchicalInstancedStaticMeshComponent* Selectcomp2;

	UPROPERTY()
	AHologramHelper* FocusActor = nullptr;

	UHierarchicalInstancedStaticMeshComponent* Focuscomp2;

	FMaterialPair()
		: SelectMaterial(nullptr), FocusMaterial(nullptr)
	{}
};


struct FSMeshMaterial
{
	TWeakObjectPtr<UMeshComponent> MeshComponent = nullptr;
	UFGColoredInstanceMeshProxy* MeshProxy = nullptr;
	TArray<UMaterialInterface*> MaterialInterfaceList;
	TArray<UMaterialInterface*> MaterialInterfaceList2;
	//TMap< TWeakObjectPtr<UMeshComponent>, TArray<UMaterialInterface*> > MeshComponentArray;
};

//USTRUCT()
struct FSavedMaterialInterfaces
{
	//GENERATED_BODY()

public:
	UPROPERTY()
	TArray<class UMaterialInterface*> MaterialInterfaces;

public:
	//FORCEINLINE ~FSavedMaterialInterfaces() = default;
};

struct FSActorMaterial
{
	bool Init = false;
	TWeakObjectPtr<AFGBuildable> Buildable = nullptr;
	TArray<FSMeshMaterial> MeshList;

	UPROPERTY()
	TMap<class UMeshComponent*, FSavedMaterialInterfaces> SavedMaterialInterfaces;
	UPROPERTY()
	bool hologramVariation = false;
	UPROPERTY()
	bool UseDefaultOutline = false;
	UPROPERTY()
	TMap<UStaticMesh*, UInstancedStaticMeshComponent*> OutlineProxyData;
	UPROPERTY()
	AFGHologram* Hologram;
	//UPROPERTY()
	//TArray<struct FInstanceHandle*> InstanceHandles;
	TArray<FInstanceHandle*> InstanceHandles;
	UPROPERTY()
	TMap<UStaticMesh*, UHierarchicalInstancedStaticMeshComponent*> copiedComponents;
	UPROPERTY()
	AHologramHelper* HologramHelper;

	FInstanceHandle Handle;

};

struct FSMaterialHandle
{
	//UMaterialInterface* Material;
	TArray<class UMaterialInterface*> Materials;
	TArray<UMaterialInterface*> MaterialInterfaceList;
	TMap<UMeshComponent*, FSMeshMaterial* > MaterialMapping;
	UMaterialInterface* Material;
	int32 Handle;
	bool hologramVariation = false;
	AFGHologram* Hologram;
	TArray<FInstanceHandle*> InstanceHandles;
	TArray<UHierarchicalInstancedStaticMeshComponent*> copiedComponents;
	AHologramHelper* HologramHelper;
};

struct FSISMNode
{
	UInstancedStaticMeshComponent* ISMComponent = nullptr;
	TArray<FSMaterialHandle*> Handles;
};

/**
 * 
 */



/*
USTRUCT()
struct FACTORYSKYLINE_API FSelectedActorInfo
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<class UMeshComponent*, FSavedMaterialInterfaces> SavedMaterialInterfaces;
	UPROPERTY()
	bool hologramVariation = false;
	UPROPERTY()
	bool UseDefaultOutline = false;
	UPROPERTY()
	TMap<UStaticMesh*, UInstancedStaticMeshComponent*> OutlineProxyData;
	UPROPERTY()
	AFGHologram* Hologram;
	//UPROPERTY()
	TArray<FInstanceHandle*> InstanceHandles;
	UPROPERTY()
	TArray<UHierarchicalInstancedStaticMeshComponent*> copiedComponents;
	UPROPERTY()
	AHologramHelper* HologramHelper;

public:
	FORCEINLINE ~FSelectedActorInfo() = default;
};
*/


USTRUCT()
struct FACTORYSKYLINE_API FComponentSavedInterfaces
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UMaterialInterface*> SavedInterfaces;

public:
	FORCEINLINE ~FComponentSavedInterfaces() = default;
};

UCLASS()
class FACTORYSKYLINE_API UFSSelection : public UObject
{
	GENERATED_BODY()
public:

	void Init();

	void Select(FSBuildable Buildable);
	bool ConnectSelect(AFGBuildable* Buildable);
	bool ConnectSelectCheckReady();
	bool RectSelectStart(const FVector2D& Start, bool SelectPositive);
	bool RectSelectCheckReady();
	bool RectSelectUpdate(FVector2D Cursor);
	void RectSelectExit(bool Valid);

	void Load(UFSDesign* DesignParam);
	void LoadSelect(UFSDesign* Design);
	void LoadSetAnchor(UFSDesign* Design);
	static void SetAutoRebuildTreeAll(bool Auto);
	void Unload();
	void BeginDestroy();

	bool IsHightLight(FSBuildable Buildable);
	void EnableHightLight(FSBuildable Buildable);
	void EnableHightLight(FSBuildable Buildable, UMaterialInterface* Material);
	void EnableHightLight(FSActorMaterial& Cache, FSBuildable Buildable, UMaterialInterface* Material);
	void DisableHightLight(FSBuildable Buildable);
	void EnableHightLightFocus(FSBuildable Buildable);
	void DisableHightLightFocus();
	void DisableAll();
	
	FSISMNode* GetISM(UFGColoredInstanceMeshProxy* MeshProxy, UMaterialInterface* Material);
	void AddInstance(UFGColoredInstanceMeshProxy* MeshProxy, UMaterialInterface* Material);
	void RemoveInstance(UFGColoredInstanceMeshProxy* MeshProxy, FSMaterialHandle* Node);
	void AddInstance(UFGColoredInstanceMeshProxy* MeshProxy, uint8 Slot);
	void RemoveInstance(UFGColoredInstanceMeshProxy* MeshProxy, uint8 Slot);

	void SpawnInitialAreaBox(AFGBuildable* Buildable);
	void ChangeAreaBox(float Size);
	void SelectBuildablesInAreaBox();
	void UpdateHighlightedActors();

	void SetMeshInstanced(UMeshComponent* MeshComp, bool Instanced) const;

	void InitMaterials();

	void HideHologram(AActor* Actor, FSActorMaterial& ActorInfo);
	void BuildStaticMeshOrigin(AFGBuildable* Buildable, UStaticMesh* StaticMesh, UMaterialInterface* Material);
	void TestMethod(AFGBuildable* Buildable);
	UInstancedStaticMeshComponent* MakeOutlineProxyMeshComponent(AActor* Buildable, UStaticMesh* StaticMesh);

	UFSDesign* Design;
	UFSConnectSelectService* ConnectSelectService = nullptr;
	UFSRectSelectService* RectSelectService = nullptr;
	FSBuildable CurrentFocus = FSBuildable();

	int LastSelectMode;
	TMap<TWeakObjectPtr<AFGBuildable>, int> RectSelectBuffer;

	TMap<FSBuildable, FSActorMaterial> ActorMaterialCache;

	TMap<FSBuildable, FSActorMaterial> ActorMaterialCache2;

	//UPROPERTY()
	//UMaterialInstanceConstant* Hologram = nullptr;

	UPROPERTY()
	TArray<UInstancedStaticMeshComponent*> GCFlag;
	TMap<TPair<UFGColoredInstanceManager*, UMaterialInterface* >, FSISMNode*> ISMMapping;
	//TMap<UMeshComponent*, FSMaterialHandle > MaterialMapping;
	TMap<UMeshComponent*, FSMaterialHandle* > MaterialMapping;
	UPROPERTY()
	TArray<AActor*> ActorList;

	//UPROPERTY()
	//TMap<AActor*, FSelectedActorInfo> SelectedMap;

	TArray<FSMaterialHandle*> HandleList;
	TMap<UMeshComponent*, FSMaterialHandle* > MaterialMapping2;

	UPROPERTY()
	TSet<UMaterialInstanceDynamic*> DynamicInstanceSet;

	UPROPERTY()
	UMaterialInstanceDynamic* SelectMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* FocusMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* InvisibleMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* CubeMaterial;

	UPROPERTY()
	UTexture* Scanline;

	UPROPERTY()
	bool MaterialisInitialized;

	//UPROPERTY()
	//TMap<AActor*, FSelectedActorInfo> SelectedMap;

	//UPROPERTY()
	//TMap<AActor*, TArray<UMaterialInterface*>> SavedInterfaceList;

	//UPROPERTY()
	//TMap<UMeshComponent*, FComponentSavedInterfaces> SavedMaterialInterfaces;
	uint32 HandleID;
	TArray<uint32>* handleIDs;
	UAbstractInstanceDataObject* Data;
	AAbstractInstanceManager* Manager;
	TArray<FInstanceHandle*> InstanceHandles;
	UStaticMesh* MeshFromHandle;
	UStaticMesh* MeshFromInstanceData;

	UPROPERTY()
	AMyActor* Cube;

	TSet<UStaticMeshComponent*> components;
	UHierarchicalInstancedStaticMeshComponent* comp2;
	TArray<int32> InstanceCount;

	UPROPERTY()
	TMap<UStaticMesh*, UInstancedStaticMeshComponent*> OutlineProxyData;

	FSBuildable BuildableEmpty = FSBuildable();

	AFGBuildable* testBuildable = nullptr;
	bool isValid = false;

	TMap<TSubclassOf< AFGBuildable >, FMaterialPair> AbstractMaterialMap;

};
