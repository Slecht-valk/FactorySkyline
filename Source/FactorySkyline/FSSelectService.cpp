// ILikeBanas


#include "FSSelectService.h"
#include "FSkyline.h"
#include "FGRecipeManager.h"
#include "Resources/FGBuildingDescriptor.h"


void UFSConnectSelectService::Init()
{
	AFSkyline* FSkyline = AFSkyline::Get(this);
	if (!FSkyline) return;

	this->World = FSkyline->World;
	this->FGController = FSkyline->FGController;

	// keep with commented out
	//this->Select = FSkyline->Select;
	this->BuildableService = FSkyline->BuildableService;

	this->ConnectSelect = new FSConnectSelect();

	this->OperatorFactory = NewObject<UFSOperatorFactory>(this);
	if (this->ConnectSelect) {
		this->ConnectSelect->NativeInit(this, OperatorFactory);
	}
	if (this->OperatorFactory) {
		this->OperatorFactory->Init();
	}
	this->ConnectSelectAsyncTask = new FAutoDeleteAsyncTask<FSConnectSelect>();
	//this->ConnectSelectAsyncTask->StartBackgroundTask();
}

void UFSConnectSelectService::RefreshRecipe()
{
	TArray< TSubclassOf< UFGRecipe > > out_recipes;
	AFGRecipeManager::Get(this)->GetAllAvailableRecipes(out_recipes);
	out_recipes.Add(LoadClass<UFGRecipe>(this, TEXT("/Game/FactoryGame/Recipes/Buildings/Recipe_RailroadTrackIntegrated.Recipe_RailroadTrackIntegrated_C")));

	for (TSubclassOf< UFGRecipe > RecipeClass : out_recipes) {
		TArray< FItemAmount > Items = UFGRecipe::GetProducts(RecipeClass);
		if (Items.Num() == 1 && Items[0].ItemClass) {
			TSubclassOf< UFGBuildingDescriptor > BuildingDescriptorClass = (TSubclassOf< UFGBuildingDescriptor >)(UFGRecipe::GetDescriptorForRecipe(RecipeClass));
			if (BuildingDescriptorClass) {
				UClass* BuildableClass = UFGBuildingDescriptor::GetBuildableClass(BuildingDescriptorClass);
				if (BuildableClass) {
					OperatorFactory->EnsureCache(BuildableClass);
				}
			}
		}
	}
}

bool UFSConnectSelectService::Ready()
{
	return this->ConnectSelect->IsReady();
}

bool UFSConnectSelectService::GetLastResult(TArray<FSBuildable>*& Result)
{
	if (!this->ConnectSelect->IsReady()) return false;
	if (this->ConnectSelect->GetResult) return false;
	this->ConnectSelect->GetResult = true;
	Result = &this->ConnectSelect->Result;
	return true;
}

bool UFSConnectSelectService::SubmitConnectSelectTask(UFSDesign* Design, FSBuildable* Buildable, int SelectType)
{
	if (!ConnectSelect->IsReady()) return false;
	if (SelectType != 1 && SelectType != 2) return false;

	ConnectSelect->Result.Empty();
	ConnectSelect->Stack.Empty();

	// TODO REWORK THIS
	//ConnectSelect->Select = Design->BuildableSet;

	//ConnectSelect->Mark = *Design->BuildableMark;

	// The new map that will use FSBuildable directly
	TMap<FSBuildable, int> Mark;

	// Iterate over BuildableMark and add the dereferenced FSBuildable to Mark
	for (TPair<FSBuildable, int>& Pair : Design->BuildableMark)
	{
		FSBuildable* ptr = &Pair.Key;
		// Dereference the FSBuildable pointer and insert it into the new map
		if (ptr != nullptr)  // Ensure pointer is valid
		{
			Mark.Add(Pair.Key, Pair.Value);
		}
	}
	ConnectSelect->Mark = Mark;

	ConnectSelect->StartNode = *Buildable;
	ConnectSelect->SearchType = OperatorFactory->GetType(Buildable);

	ConnectSelect->TaskType = SelectType;
	ConnectSelect->StartTask();

	//std::this_thread::sleep_for(std::chrono::nanoseconds(100000000));
	//this->ConnectSelect->DoWork();
	//this->ConnectSelectAsyncTask = new FAutoDeleteAsyncTask<FSConnectSelect>();
	this->ConnectSelectAsyncTask->StartBackgroundTask();

	return true;
}

void UFSConnectSelectService::TerminalCurrentTask()
{
	ConnectSelect->TerminalTask();
}

void UFSConnectSelectService::BeginDestroy()
{
	/*
	if (!ConnectSelectAsyncTask.IsWorkDone()) {
		ConnectSelectAsyncTask.Cancel();
		ConnectSelectAsyncTask.WaitCompletionWithTimeout(5000);
		ConnectSelectAsyncTask.EnsureCompletion(false);

		ConnectSelectAsyncTask.GetTask().ShutDown();
	}
	*/
	//this->World = nullptr;
	//this->OperatorFactory = nullptr;

	//ConnectSelectAsyncTask->Abandon();
	//delete ConnectSelectAsyncTask;
	//delete this;
	//ConnectSelectAsyncTask = nullptr;

	//this->ConnectSelect->NeedRunning = false;
	//this->ConnectSelect->TaskType = 3;
	//ConnectSelectAsyncTask.GetTask().ShutDown();
	//ConnectSelectAsyncTask = nullptr;

	Super::BeginDestroy();
}

void UFSConnectSelectService::FinishDestroy()
{
	//delete ConnectSelectAsyncTask;


	//ConnectSelect->ShutDown();
	//this->ConnectSelect->NeedRunning = false;
	//ConnectSelectAsyncTask = nullptr;
	Super::FinishDestroy();
}

void FSConnectSelect::NativeInit(UObject* WorldContext, UFSOperatorFactory* OperatorFactory)
{
	AFSkyline* FSkyline = AFSkyline::Get(WorldContext);
	if (!FSkyline) return;

	this->World = FSkyline->World;
	this->OperatorFactory = OperatorFactory;

	this->Result.Empty();
	this->Event = FGenericPlatformProcess::GetSynchEventFromPool(false);
	this->NeedRunning = true;
	this->Ready = false;
	this->ForceStop = false;
	this->TaskType = 0;
	this->IsInited = true;
}


void FSConnectSelect::DoWork()
{

	//while (NeedRunning) {
		if (TaskType == 3) {
			//break;
		}
		if (TaskType == 0) {
			this->Ready = true;
			//Event->Wait();
		}
		else if (TaskType == 1) {
			Search_Positive();
			TaskType = 0;
		}
		else if (TaskType == 2) {
			Search_Nagetive();
			TaskType = 0;
		}
	//}

}

void FSConnectSelect::ShutDown()
{
	ForceStop = true;
	NeedRunning = false;
	Event->Trigger();
}

void FSConnectSelect::StartTask()
{
	if (!this->Ready) return;
	ForceStop = false;
	Ready = false;
	GetResult = false;
	Event->Trigger();
}

void FSConnectSelect::TerminalTask()
{
	ForceStop = true;
}

bool FSConnectSelect::IsReady()
{
	return this->Ready;
}

void FSConnectSelect::Search_Positive()
{
	if (Select.Contains(StartNode)) return;
	TArray<FSBuildable > Array;

	for (TPair< FSBuildable, int>&Pair : Mark)
		if (Pair.Value == 0) Select.Add(Pair.Key);

	Stack.Add(StartNode);
	Select.Add(StartNode);
	for (int i = 0; i < Stack.Num(); i++) {
		if (ForceStop) break;

		FSBuildable* Buildable = &Stack[i];
		if (Buildable) {
			Result.Add(Stack[i]);
			//SML::Logging::info(*Buildable->GetFullName());

			Array.Empty(128);

			// REWORK THIS, DO WE NEED THIS?
			//OperatorFactory->GetSelectConnectList(Buildable, Array);

			for (FSBuildable& Ptr : Array) {
				FSBuildable* Next = &Ptr;

				if (Next && !Select.Contains(Ptr) && OperatorFactory->GetType(Next) == SearchType) {
					Select.Add(Ptr);
					Stack.Add(Ptr);
				}
			}
		}
	}
}

bool FSConnectSelect::BFS_Nagetive(FSBuildable StartPtr, TArray<FSBuildable >& Result)
{
	Stack.Empty();
	Stack.Add(StartPtr);
	TArray<FSBuildable > Array;
	for (int i = 0; i < Stack.Num(); i++) {
		if (ForceStop) break;

		FSBuildable* Buildable = &Stack[i];
		if (Buildable) {
			Result.Add(Stack[i]);

			Array.Empty(128);

			// REWORK THIS, DO WE NEED THIS?
			//OperatorFactory->GetSelectConnectList(Buildable, Array);

			for (FSBuildable& Ptr : Array) {
				FSBuildable* Next = &Ptr;

				if (Next && Select.Contains(Ptr) && OperatorFactory->GetType(Next) == SearchType) {
					int* p = Mark.Find(Ptr);
					if (p && *p == 2) {
						for (FSBuildable& Nage : Stack) {
							int *q = Mark.Find(Nage);
							if (q) *q = 2;
							else Mark.Add(Nage, 2);
						}
						return false;
					}
					if (!p) {
						Mark.Add(Ptr, 1);
						Stack.Add(Ptr);
					}
				}
			}
		}
	}

	return true;
}

void FSConnectSelect::Search_Nagetive()
{
	if (!Select.Contains(StartNode)) return;
	Select.Remove(StartNode);

	FSBuildable* Buildable = &StartNode;
	if (!Buildable) return;

	Result.Add(StartNode);
	TArray<FSBuildable > Array;

	// REWORK DO WE NEED THIS?
	//OperatorFactory->GetSelectConnectList(Buildable, Array);

	for (FSBuildable& Ptr : Array) {
		FSBuildable* Buildable = &Ptr;
		if (Buildable && Select.Contains(Ptr) && !Mark.Find(Ptr) && OperatorFactory->GetType(Buildable) == SearchType) {
			TArray<FSBuildable > BfsResult;
			Mark.Add(Ptr, 1);
			if (BFS_Nagetive(Ptr, BfsResult)) Result.Append(BfsResult);
		}
	}
}





void UFSRectSelectService::Init()
{
	AFSkyline* FSkyline = AFSkyline::Get(this);
	if (!FSkyline) return;

	this->World = FSkyline->World;
	this->FGController = FSkyline->FGController;

	this->BuildableService = FSkyline->BuildableService;
	this->RectSelect = new FSRectSelect();

	if (this->RectSelect) {
		this->RectSelect->NativeInit(this);
	}

	this->RectSelectAsyncTask = new FAutoDeleteAsyncTask<FSRectSelect>();
	//this->RectSelectAsyncTask->StartBackgroundTask();
}

bool UFSRectSelectService::Ready()
{
	return this->RectSelect->IsReady();
}

bool UFSRectSelectService::GetLastResult(TArray<TWeakObjectPtr<AFGBuildable> >*& Positive, TArray<TWeakObjectPtr<AFGBuildable> >*& Nagetive)
{
	if (!this->RectSelect->IsReady()) return false;
	if (this->RectSelect->GetResult) return false;
	this->RectSelect->GetResult = true;
	Positive = &this->RectSelect->Positive;
	Nagetive = &this->RectSelect->Nagetive;
	return true;
}

bool UFSRectSelectService::StartRectSelect(UFSDesign* Design, const FVector2D& Start, bool LeftClick)
{
	if (!RectSelect->IsReady()) return false;

	RectSelect->TaskType = 1;
	RectSelect->Design = Design;
	RectSelect->LeftClick = LeftClick;
	RectSelect->Start = Start;
	RectSelect->Last = Start;

	RectSelect->Load();
	RectSelect->StartTask();

	//std::this_thread::sleep_for(std::chrono::nanoseconds(100000000));
	//this->RectSelect->DoWork();
	//this->RectSelectAsyncTask = new FAutoDeleteAsyncTask<FSRectSelect>();
	this->RectSelectAsyncTask->StartBackgroundTask();

	return true;
}

bool UFSRectSelectService::QueryRectSelect(int x, int y)
{
	if (!RectSelect->IsReady()) return false;

	RectSelect->TaskType = 2;
	RectSelect->End.Pos[0] = x;
	RectSelect->End.Pos[1] = y;

	RectSelect->StartTask();

	//this->RectSelect->DoWork();
	//this->RectSelectAsyncTask = new FAutoDeleteAsyncTask<FSRectSelect>();
	this->RectSelectAsyncTask->StartBackgroundTask();

	return true;
}

void UFSRectSelectService::TerminalCurrentTask()
{
	RectSelect->TerminalTask();
}

void UFSRectSelectService::BeginDestroy()
{
	/*
	if (RectSelectAsyncTask.GetTask().IsInited) {
		RectSelectAsyncTask.GetTask().ShutDown();
		RectSelectAsyncTask.EnsureCompletion(false);
	}
	*/
	Super::BeginDestroy();
}
void UFSRectSelectService::FinishDestroy()
{
	Super::FinishDestroy();
}



void FSRectSelect::NativeInit(UObject* WorldContext)
{
	AFSkyline* FSkyline = AFSkyline::Get(WorldContext);
	if (!FSkyline) return;

	this->World = FSkyline->World;
	this->FGController = FSkyline->FGController;

	this->Event = FGenericPlatformProcess::GetSynchEventFromPool(false);
	this->NeedRunning = true;
	this->Ready = false;
	this->TaskType = 0;
	this->IsInited = true;
}

void FSRectSelect::DoWork()
{
	//while (NeedRunning) {
		if (TaskType == 0) {
			this->Ready = true;
			//Event->Wait();
		}
		else if (TaskType == 1) {
			Build();
			TaskType = 0;
		}
		else if (TaskType == 2) {
			Query();
			TaskType = 0;
		}
	//}
}

void FSRectSelect::Load()
{
	Tree.Reset = false;
}

void FSRectSelect::ShutDown()
{
	Tree.Reset = true;
	NeedRunning = false;
	Event->Trigger();
}

void FSRectSelect::StartTask()
{
	if (!this->Ready) return;
	Ready = false;
	GetResult = false;
	Event->Trigger();
}

void FSRectSelect::TerminalTask()
{
	Tree.Reset = true;
}

bool FSRectSelect::IsReady()
{
	return this->Ready;
}

void FSRectSelect::Build()
{
	Positive.Empty();
	Nagetive.Empty();
	BuildableSet.Empty();

	// TODO REWORK THIS
	
	for (TObjectIterator<AFGBuildable>It; It; ++It) {
		AFGBuildable* BuildableActor = *It;
		FSBuildable Buildable;
		Buildable.Buildable = BuildableActor;
		if (LeftClick) {

			if (!Design->IsElementSelected(Buildable))
				BuildableSet.Add(BuildableActor);
		}
		else {
			if (Design->IsElementSelected(Buildable))
				BuildableSet.Add(BuildableActor);
		}
	}
	
	Tree.BuildTree(BuildableSet, FGController);
}

void FSRectSelect::Query()
{
	Positive.Empty();
	Nagetive.Empty();
	if (End == Last) return;
	Int2D StartOld = Start;
	Int2D StartNew = Start;
	Int2D EndOld = Last;
	Int2D EndNew = End;
	Last = End;
	if (StartOld.Pos[0] > EndOld.Pos[0]) Swap(StartOld.Pos[0], EndOld.Pos[0]);
	if (StartOld.Pos[1] > EndOld.Pos[1]) Swap(StartOld.Pos[1], EndOld.Pos[1]);
	if (StartNew.Pos[0] > EndNew.Pos[0]) Swap(StartNew.Pos[0], EndNew.Pos[0]);
	if (StartNew.Pos[1] > EndNew.Pos[1]) Swap(StartNew.Pos[1], EndNew.Pos[1]);

	if (LeftClick) Tree.QueryDelta(Positive, Nagetive, StartOld, EndOld, StartNew, EndNew);
	else Tree.QueryDelta(Nagetive, Positive, StartOld, EndOld, StartNew, EndNew);
}
