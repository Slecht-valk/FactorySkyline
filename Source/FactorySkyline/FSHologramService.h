// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FactorySkyline/FSBuildableService.h"
#include "FactorySkyline/FSDesign.h"
#include "Operators/FSBuildableOperator.h"
#include "Equipment/FGBuildGun.h"
#include "FGBuildableSubsystem.h"

#include<chrono>
#include<thread>

#include "FSHologramService.generated.h"

class FSHologramTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FSHologramTask>;
public:
	FSHologramTask(bool Init, UObject* W) {
		IsInited = Init; 
		Event = nullptr; 
		WorldContext = W;
	}

	void NativeInit(UObject* WorldContext);
	void DoWork();
	void Load();
	void ShutDown();

	void StartTask();
	void TerminalTask();
	bool IsReady();

	void Build();
	void Query();
	KDTree2D Tree;

	UWorld* World;

	FEvent* Event;

	bool IsInited;
	bool NeedRunning;
	bool Ready;
	bool GetResult;
	int TaskType;

	UObject* WorldContext;

	Int2D Start;
	Int2D Last;
	Int2D End;
	bool LeftClick;

	UFSDesign* Design;
	APlayerController* FGController;
	TSet<TWeakObjectPtr<AFGBuildable>> BuildableSet;
	TArray< TWeakObjectPtr<AFGBuildable> > Positive;
	TArray< TWeakObjectPtr<AFGBuildable> > Nagetive;

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FSHologramTask, STATGROUP_ThreadPoolAsyncTasks); }

	~FSHologramTask() { if (Event) FGenericPlatformProcess::ReturnSynchEventToPool(Event); }
};

/**
 *
 */
UCLASS()
class FACTORYSKYLINE_API UFSHologramService : public UObject
{
	GENERATED_BODY()
public:

	void Init();

	bool Ready();
	void TerminalCurrentTask();

	virtual void BeginDestroy();
	virtual void FinishDestroy();

	UWorld* World = nullptr;
	AFGPlayerController* FGController = nullptr;
	//AFGBuildGun* FGBuildGun = nullptr;

	UFSHologramService* BuildableService = nullptr;

	FSHologramTask* RectSelect = nullptr;
	FAutoDeleteAsyncTask<FSHologramTask>* AsyncTask;
};
