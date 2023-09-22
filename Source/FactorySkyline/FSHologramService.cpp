// ILikeBanas


#include "FSHologramService.h"
#include "FSkyline.h"
#include "FGRecipeManager.h"
#include "Resources/FGBuildingDescriptor.h"


void UFSHologramService::Init()
{
	//AFSkyline* FSkyline = AFSkyline::Get(this);
	//if (!FSkyline) return;

	//this->World = FSkyline->World;
	//this->FGController = FSkyline->FGController;

	//this->BuildableService = FSkyline->BuildableService;

	//this->RectSelect = new FSHologramTask(true);
	//this->RectSelect->IsInited = true;


	//if (this->RectSelect) {
		//this->RectSelect->NativeInit(this);
	//}

	//this->AsyncTask = new FAutoDeleteAsyncTask<FSHologramTask>();

	FAutoDeleteAsyncTask<FSHologramTask>* task = new FAutoDeleteAsyncTask<FSHologramTask>(true, this);

	//bool init = this->RectSelect ->IsInited;
	//this->RectSelect->IsInited = true;

	task->StartBackgroundTask();

	/*
	TSharedPtr<FSHologramTask, ESPMode::ThreadSafe> MyTask = MakeShareable(new FSHologramTask(true));


	FGraphEventRef TaskGraphEvent = FFunctionGraphTask::CreateAndDispatchWhenReady(
		[=]()
	{
		MyTask->DoWork();
	},
		TStatId(),
		nullptr,
		ENamedThreads::AnyThread
		);
		*/
}



bool UFSHologramService::Ready()
{
	return this->RectSelect->IsReady();
}

void UFSHologramService::TerminalCurrentTask()
{
	RectSelect->TerminalTask();
}

void UFSHologramService::BeginDestroy()
{

	Super::BeginDestroy();
}
void UFSHologramService::FinishDestroy()
{
	Super::FinishDestroy();
}



void FSHologramTask::NativeInit(UObject* WorldContext)
{
	AFSkyline* FSkyline = AFSkyline::Get(WorldContext);
	if (!FSkyline) return;

	this->World = FSkyline->World;
	this->FGController = FSkyline->FGController;

	this->Event = FGenericPlatformProcess::GetSynchEventFromPool(false);
}

void FSHologramTask::DoWork()
{
	if (this->IsInited) {
		std::string  str = "Is Initiated";
		str.append("\n");


		std::wstring temp = std::wstring(str.begin(), str.end());
		LPCWSTR wideString = temp.c_str();
		OutputDebugStringW(wideString);
	}else {
		std::string  str = "Isnt Initiated";
		str.append("\n");


		std::wstring temp = std::wstring(str.begin(), str.end());
		LPCWSTR wideString = temp.c_str();
		OutputDebugStringW(wideString);
	}
	/*
	std::string  str = "Initialized";
	str.append("\n");


	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugStringW(wideString);
	*/

	AFSkyline* FSkyline = AFSkyline::Get(WorldContext);
	if (!FSkyline) return;


	for (TWeakObjectPtr<AFGBuildable> BuildablePtr : FSkyline->Select->Design->BuildableSet) {
		if (BuildablePtr.Get()) {
			AFGBuildable* Buildable = BuildablePtr.Get();

			FSkyline->Select->TestMethod(Buildable);

			//TArray<FInstanceHandle*>InstanceHandles = Buildable->mInstanceHandles;

			//FSkyline->Select->EnableHightLight(Buildable);
		}
		if (Design->Anchor.Get()) {
			//FSkyline->Select->EnableHightLight(FSkyline->Select->Design->Anchor.Get(), FSkyline->Select->FocusMaterial);
		}
	}
}

void FSHologramTask::Load()
{
}

void FSHologramTask::ShutDown()
{
	Event->Trigger();
}

void FSHologramTask::StartTask()
{
	Event->Trigger();
}

void FSHologramTask::TerminalTask()
{

}

bool FSHologramTask::IsReady()
{
	return this->Ready;
}

void FSHologramTask::Build()
{
}

void FSHologramTask::Query()
{
}

/*
FSHologramTask::FSHologramTask()
{
	// Initialize member variables here

	std::string  str = "Initialized";
	str.append("\n");


	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugStringW(wideString);

}
*/