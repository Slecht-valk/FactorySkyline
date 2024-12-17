// ILikeBanas


#include "FSController.h"
#include "FSkyline.h"
#include "FSBuilder.h"
#include "FSDesign.h"
#include "FSSelection.h"
#include "UI/FSkylineUI.h"
#include "FGPlayerController.h"
#include "FGCharacterPlayer.h"
#include "FGHealthComponent.h"
#include "UI/FGVirtualCursorFunctionLibrary.h"
#include "UI/FGGameUI.h"
#include "Equipment/FGBuildGun.h"
#include "Equipment/FGWeapon.h"
#include "Framework/Application/SlateApplication.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/ExponentialHeightFogComponent.h"

#include "Equipment/FGBuildGunDismantle.h"

#include "Tickable.h"
#include "HAL/PlatformProcess.h" 
#include "Ticker.h"

//#include "FGLightweightBuildableSubsystem.h"

#include <string>


void AFSController::Init() {
	AFSkyline* FSkyline = AFSkyline::Get(this);

	this->World = FSkyline->World;
	this->WorldSettings = FSkyline->WorldSettings;
	this->FGController = FSkyline->FGController;
	this->ResourceSinkSubsystem = AFGResourceSinkSubsystem::Get(FSkyline->World);
	this->FGUI = FSkyline->GameUI;
	this->HUD = FSkyline->HUD;
	GetPlayer();

	this->SkylineUI = FSkyline->SkylineUI;
	this->Builder = FSkyline->Builder;
	this->Select = FSkyline->Select;

	if (!this->Etc) this->Etc = NewObject<UFSEtc>(this);
	this->FSInput = NewObject<UFSInput>(this);

	this->Etc->Init();
	this->FSInput->Init();
	this->Inventory.Init(FSkyline->SplineHologramFactory);

	if (this->DesignRoot) {
		SkylineUI->Turtioul = false;
		DesignRoot->LoadInit(SkylineUI, nullptr);
	}
	else {
		SkylineUI->Turtioul = true;
		DesignRoot = UFSDesignMenu::CreateDesignMenu(this, false);
		DesignRoot->SkylineUI = SkylineUI;
	}

	UE_LOG(LogSkyline, Verbose, TEXT("Initializing global Keybinds"));
	
	FSInput->BindDelegate("MenuKey", FSInputEvent::FS_Pressed, this, &AFSController::onCallMenu);
	FSInput->BindDelegate("EscapeKey", FSInputEvent::FS_Pressed, this, &AFSController::onEscPressed);
	//FSInput->BindDelegate("LeftControlKey", FSInputEvent::FS_Pressed, this, &AFSController::onLeftCtrlPressed);
	//FSInput->BindDelegate("LeftControlKey", FSInputEvent::FS_Repeat, this, &AFSController::onLeftCtrlPressedRepeat);
	//FSInput->BindDelegate("LeftControlKey", FSInputEvent::FS_Released, this, &AFSController::onLeftCtrlReleased);
	FSInput->BindDelegate("FlyingKey", FSInputEvent::FS_Pressed, this, &AFSController::onDoubleSpacePressed);
	FSInput->BindDelegate("TurboSprintKey", FSInputEvent::FS_Pressed, this, &AFSController::onDoubleShiftPressed);
	FSInput->BindDelegate("ScrollUpKey", FSInputEvent::FS_Pressed, this, &AFSController::onMouseScrollUp);
	FSInput->BindDelegate("ScrollDownKey", FSInputEvent::FS_Pressed, this, &AFSController::onMouseScrollDown);
	FSInput->BindDelegate("LeftMouseKey", FSInputEvent::FS_Pressed, this, &AFSController::onLeftMouseDown);
	FSInput->BindDelegate("LeftMouseKey", FSInputEvent::FS_Released, this, &AFSController::onLeftMouseUp);
	FSInput->BindDelegate("RightMouseKey", FSInputEvent::FS_Pressed, this, &AFSController::onRightMouseDown);
	FSInput->BindDelegate("RightMouseKey", FSInputEvent::FS_Repeat, this, &AFSController::onRightMouseDownRepeat);
	FSInput->BindDelegate("RightMouseKey", FSInputEvent::FS_Released, this, &AFSController::onRightMouseUp);
	FSInput->BindDelegate("ActionShortcut1Key", FSInputEvent::FS_Pressed, this, &AFSController::onActionShortcut1);
	FSInput->BindDelegate("DeleteKey", FSInputEvent::FS_Pressed, this, &AFSController::onDesignDelete);
	
	
	LastLeftCtrlPressed = 0.0f;
	LastRightMouseDown = 0.0f;

	RectangleSelectMode = false;
	LastShowMouseCursor = false;

	LeftCtrlPressed = false;
	LeftMousePressed = false;
	RightMousePressed = false;
	CallMenuPressed = false;

	this->Design = nullptr;

	AFGCharacterPlayer::OnEquipmentEquipped.AddUObject<AFSController>(this, &AFSController::onPlayerEquipmentEquipped);
	AFGCharacterPlayer::OnEquipmentUnequipped.AddUObject<AFSController>(this, &AFSController::onPlayerEquipmentUnequipped);

	// TODO Most recent patch for u6 broke this, find alternative?
	
	
	FScriptDelegate Func1;
	Func1.BindUFunction(this, FName("onBuildGunRecipeChanged"));
	FGBuildGun->mOnRecipeChanged.Add(Func1);
	

	FScriptDelegate Func2;
	Func2.BindUFunction(this, FName("onBuildGunStateChanged"));
	FGBuildGun->mOnStateChanged.Add(Func2);
	
	

	// TODO This attempt at fix didn't work either
	/*
	if (FGBuildGun != nullptr) {
		FGBuildGun->mOnRecipeChanged.AddDynamic(this, &AFSController::onBuildGunRecipeChanged);

		FGBuildGun->mOnStateChanged.AddDynamic(this, &AFSController::onBuildGunStateChanged);
	}
	*/

	FOnActorSpawned::FDelegate ActorSpawnedDelegate = FOnActorSpawned::FDelegate::CreateUObject(this, &AFSController::OnActorSpawned);
	this->World->AddOnActorSpawnedHandler(ActorSpawnedDelegate);

	this->SetActorTickEnabled(true);
	Timer.Start();
}

void AFSController::OnActorSpawned(AActor* InActor)
{
	/*
	AFGHologram* buildableHologram = Cast<AFGHologram>(InActor);
	if (buildableHologram) {
		
		std::string  str;
		//str.append("\n");
		str.append("buildableHologram SPAWNED!\n");
		std::wstring temp = std::wstring(str.begin(), str.end());
		LPCWSTR wideString = temp.c_str();
		OutputDebugStringW(wideString);
		
	}
	*/

	//do something 

	AFGBuildable* buildable = Cast<AFGBuildable>(InActor);

	if (buildable) {
		if (this->Design) {
			if (this->Design->AddConstructedBuildable) {

				// TODO REWORK THIS
				//this->Design->AddElement(buildable);
				//this->Design->BuildableMark.Add(buildable, 1);
				
				//this->Select->EnableHightLight(buildable, this->Select->SelectMaterial);

			}
		}
		/*
		std::string  str;
		//str.append("\n");
		str.append("BUILDABLE SPAWNED!\n");
		std::wstring temp = std::wstring(str.begin(), str.end());
		LPCWSTR wideString = temp.c_str();
		OutputDebugStringW(wideString);
		*/
	}

}

void AFSController::onPlayerEquipmentEquipped(AFGCharacterPlayer* Player, AFGEquipment* Equipment) {

	this->localPlayer = Player;

	if (SkylineUI->IsActive) {
		if (Equipment == this->FGBuildGun) {
			if (SkylineUI->GettingStart->AccquireMenuWidget()->GetParent()) {
				SkylineUI->UnexpandStart(false);
			}
			if (!SkylineUI->Turtioul) SkylineUI->FoldDesign();
		}
	}
}

void AFSController::onPlayerEquipmentUnequipped(AFGCharacterPlayer* Player, AFGEquipment* Equipment) {
	if (SkylineUI->IsActive) {
		if (Equipment == this->FGBuildGun) {
			if (State == FSState::Select || State == FSState::SetAnchor || State == FSState::Copy || State == FSState::SetItem || State == FSState::SetAreaAnchor) {
				SetOpenState(false);
			}
		}
	}
}

void AFSController::onBuildGunRecipeChanged(TSubclassOf<class UFGRecipe> newRecipe)
{
}

void AFSController::onBuildGunStateChanged(EBuildGunState newState)
{
	if (newState == EBuildGunState::BGS_DISMANTLE) {
		//UFGBuildGunStateDismantle* state = Cast< UFGBuildGunStateDismantle>(this->FGBuildGun->mDismantleStateClass);
		//fgcheck(state);

		//return;

		// kills all inputs for dismantle state
		/*
		for (TObjectIterator<UInputComponent> It; It; ++It) {
			if ((*It)->GetOuter() == this->FGBuildGun) {
				this->FGController->PopInputComponent(*It);
			}
		}
		*/

	}

	if (SkylineUI->IsActive) {
		if (State == FSState::Select || State == FSState::SetAnchor || State == FSState::Copy || State == FSState::SetItem || State == FSState::SetAreaAnchor) {
			if (newState == EBuildGunState::BGS_DISMANTLE) {
				for (TObjectIterator<UInputComponent> It; It; ++It) {
					if ((*It)->GetOuter() == this->FGBuildGun) {
						this->FGController->PopInputComponent(*It);
					}
				}
				return;
			}
		}
	}

	
	if (SkylineUI->IsActive) {
		if (State == FSState::Select || State == FSState::SetAnchor || State == FSState::Copy || State == FSState::SetItem || State == FSState::SetAreaAnchor) {
			if (newState == EBuildGunState::BGS_NONE) return;
			if (newState == EBuildGunState::BGS_MENU) {
				UFGBuildGunState* StateVar = this->FGBuildGun->GetBuildGunStateFor(newState);
				StateVar->EndState();
				for (TObjectIterator<UInputComponent> It; It; ++It) {
					if ((*It)->GetOuter() == this->FGBuildGun) {
						this->FGController->PopInputComponent(*It);
					}
				}
			}
			else {
				this->FGBuildGun->GotoMenuState();
				GetPlayer()->UpdateHUDCrosshair();
			}
		}
	}
	
}

void AFSController::SetOpenState(bool RestoreEquipment)
{
	if (State == FSState::Select) {
		ExitSelectMode(RestoreEquipment);
	}

	if (State == FSState::SetAreaAnchor) {
		ExitSelectMode(RestoreEquipment);
	}

	if (State == FSState::SetAnchor) {
		ExitSetAnchorMode(RestoreEquipment);
	}
	if (State == FSState::Copy) {
		ExitCopyMode(RestoreEquipment);
	}
	if (State == FSState::SetItem) {
		ExitSetItemMode(RestoreEquipment);
	}

	LeftCtrlPressed = false;
	SetLeftMousePressed(false);
	SetRightMousePressed(false);
}

void AFSController::StartRectangleSelectMode()
{
	LastShowMouseCursor = IsShowMouseCursor();

	ClearFocusBuilding();
	ShowMouseCursor();

	Cast<UWidget>(SkylineUI->SelectRect->Box)->SetVisibility(ESlateVisibility::Collapsed);

	SkylineUI->SelectRect->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	RectangleSelectMode = true;
	RectangleSelecting = false;
}

void AFSController::StartRectangleSelect(const FVector2D& Start, bool IsPositive)
{
	if (RectangleSelecting) return;
	if (!Select->RectSelectStart(Start, IsPositive)) return;

	RectangleSelecting = true;
	RectanglePositive = IsPositive;
	SelectStart = Start;

	GetPlayer()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Custom);

	float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
	SkylineUI->SelectRect->PanelSlot->SetPosition(Start/ Scale);
	SkylineUI->SelectRect->PanelSlot->SetSize(FVector2D(0.0f, 0.0f));

	Cast<UWidget>(SkylineUI->SelectRect->Box)->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

}

void AFSController::EndRectangleSelect(bool Valid)
{
	if (!RectangleSelecting) return;
	RectangleSelecting = false;

	Cast<UWidget>(SkylineUI->SelectRect->Box)->SetVisibility(ESlateVisibility::Collapsed);

	Select->RectSelectExit(Valid);
}

void AFSController::ExitRectangleSelectMode()
{
	
	EndRectangleSelect(false);

	if (SkylineUI->SelectRect->GetVisibility() == ESlateVisibility::SelfHitTestInvisible) {
		SkylineUI->SelectRect->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (RectangleSelectMode) {
		if (SkylineUI->IsActive && State == FSState::Select) {
			if (LastShowMouseCursor) {
				ShowMouseCursor();
			}
			else {
				HideMouseCursor();
			}
		}
		RectangleSelectMode = false;
		LastShowMouseCursor = false;
	}
	
	CheckFlying();
}

void AFSController::StartSelectMode()
{
	
	if (State != FSState::Open) SetOpenState(true);

	if (GetPlayer()->IsBuildGunEquipped()) GetPlayer()->ToggleBuildGun();

	State = FSState::Select;
	this->SkylineUI->OperatingWidget->OnTab2Click();

	EquipBuilder();
	PopFGUI();

	HideMouseCursor();

	SelectWaitingResult = false;
	ConnectSelectMode = true;
	ChangeConnectSelectMode();

	this->SkylineUI->RestoreHighLightMapping();
	
	// visible ui elements

	Cast<UWidget>(this->SkylineUI->OperatingHelper)->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	this->SkylineUI->PressCtrlMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	this->SkylineUI->HoldCtrlMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->SelectScrollMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->SelectMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	
	this->SkylineUI->SelectAnchorMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->GotoBuildMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->ExitMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	this->Select->LoadSelect(this->Design);
	FSBuildable Empty;
	CurrentFocusBuilding = Empty;
	
}


void AFSController::ExitSelectMode(bool RestoreEquipment)
{
	ExitRectangleSelectMode();

	Cast<UWidget>(this->SkylineUI->OperatingHelper)->SetVisibility(ESlateVisibility::Hidden);

	this->SkylineUI->PressCtrlMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->HoldCtrlMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->SelectScrollMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->SelectMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->SelectAnchorMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->GotoBuildMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->ExitMapping->SetVisibility(ESlateVisibility::Collapsed);

	this->ClearFocusBuilding();
	this->Select->Unload();

	SelectOne = false;
	State = FSState::Open;

	if (RestoreEquipment) UnequipBuilder();
}

void AFSController::StartCopyMode()
{
	if (State != FSState::Open) SetOpenState(true);

	if (GetPlayer()->IsBuildGunEquipped()) GetPlayer()->ToggleBuildGun();

	State = FSState::Copy;
	this->SkylineUI->OperatingWidget->OnTab3Click();

	EquipBuilder();
	PopFGUI();

	HideMouseCursor();

	this->SkylineUI->RestoreHighLightMapping();

	Cast<UWidget>(this->SkylineUI->OperatingHelper)->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	this->SkylineUI->PressCtrlMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->CopyScrollMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->CopyMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->BackToSelectMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->ExitMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	this->Builder->Load(this->Design, this->FullPreview);
}

void AFSController::ExitCopyMode(bool RestoreEquipment)
{
	this->SkylineUI->RepeatWidget->SetVisibility(ESlateVisibility::Hidden);

	Cast<UWidget>(this->SkylineUI->OperatingHelper)->SetVisibility(ESlateVisibility::Hidden);

	this->SkylineUI->PressCtrlMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->CopyScrollMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->CopyMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->SetRepeatMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->BackToSelectMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->ExitMapping->SetVisibility(ESlateVisibility::Collapsed);

	Cast<UWidget>(this->SkylineUI->ItemBox)->SetVisibility(ESlateVisibility::Collapsed);

	this->Builder->Unload();

	BuildOne = false;
	CopyPosition = false;
	CopyConsole = false;
	BuildRepeatCallBack = false;
	State = FSState::Open;

	if (RestoreEquipment) UnequipBuilder();
}

void AFSController::StartSetAnchorMode()
{
	if (State != FSState::Open) SetOpenState(true);

	if (GetPlayer()->IsBuildGunEquipped()) GetPlayer()->ToggleBuildGun();

	State = FSState::SetAnchor;
	this->SkylineUI->OperatingWidget->OnTab3Click();

	EquipBuilder();
	PopFGUI();

	HideMouseCursor();

	this->SkylineUI->RestoreHighLightMapping();

	Cast<UWidget>(this->SkylineUI->OperatingHelper)->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	this->SkylineUI->PressCtrlMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->SetAnchorMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->BackToSelectMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->ExitMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	this->Select->LoadSetAnchor(this->Design);
	FSBuildable Empty;
	CurrentFocusBuilding = Empty;
}

void AFSController::ExitSetAnchorMode(bool RestoreEquipment)
{
	Cast<UWidget>(this->SkylineUI->OperatingHelper)->SetVisibility(ESlateVisibility::Hidden);

	this->SkylineUI->PressCtrlMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->SetAnchorMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->BackToSelectMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->ExitMapping->SetVisibility(ESlateVisibility::Collapsed);

	this->Select->Unload();

	State = FSState::Open;

	if (RestoreEquipment) UnequipBuilder();
}

void AFSController::StartSetAreaAnchorMode() {

	if (State != FSState::Open) SetOpenState(true);

	if (GetPlayer()->IsBuildGunEquipped()) GetPlayer()->ToggleBuildGun();

	State = FSState::SetAreaAnchor;
	this->SkylineUI->OperatingWidget->OnTab4Click();

	EquipBuilder();
	PopFGUI();

	HideMouseCursor();

	SelectWaitingResult = false;
	ConnectSelectMode = true;
	ChangeConnectSelectMode();

	this->SkylineUI->RestoreHighLightMapping();

	Cast<UWidget>(this->SkylineUI->OperatingHelper)->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	this->SkylineUI->PressCtrlMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->HoldCtrlMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->SelectScrollMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->SelectMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->SelectAnchorMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->GotoBuildMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->ExitMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	this->Select->LoadSetAnchor(this->Design);
	FSBuildable Empty;
	CurrentFocusBuilding = Empty;

}

void AFSController::StartSetItemMode()
{
	if (State != FSState::Open) SetOpenState(true);

	if (GetPlayer()->IsBuildGunEquipped()) GetPlayer()->ToggleBuildGun();

	State = FSState::SetItem;

	EquipBuilder();
	PopFGUI();

	HideMouseCursor();

	this->SkylineUI->RestoreHighLightMapping();

	Cast<UWidget>(this->SkylineUI->OperatingHelper)->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	this->SkylineUI->PressCtrlMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->SetCircleCenterMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	this->SkylineUI->ExitMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	this->Select->Load(this->Design);
	FSBuildable Empty;
	CurrentFocusBuilding = Empty;
}

void AFSController::ExitSetItemMode(bool RestoreEquipment)
{
	Cast<UWidget>(this->SkylineUI->OperatingHelper)->SetVisibility(ESlateVisibility::Hidden);

	this->SkylineUI->PressCtrlMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->SetCircleCenterMapping->SetVisibility(ESlateVisibility::Collapsed);
	this->SkylineUI->ExitMapping->SetVisibility(ESlateVisibility::Collapsed);

	this->Select->Unload();

	State = FSState::Open;

	if (RestoreEquipment) UnequipBuilder();
	FSBuildable Empty;
	CurrentFocusBuilding = Empty;
}

void AFSController::SetLeftMousePressed(bool Pressed)
{
	LeftMousePressed = Pressed;

	if (State == FSState::Select)
		if (SkylineUI->SelectMapping->IsHighLight != Pressed) {
			if (Pressed) SkylineUI->SetHighLightMapping(SkylineUI->SelectMapping);
			else SkylineUI->RestoreHighLightMapping();
		}

	if (State == FSState::Copy)
		if (SkylineUI->CopyMapping->IsHighLight != Pressed) {
			if (Pressed) SkylineUI->SetHighLightMapping(SkylineUI->CopyMapping);
			else SkylineUI->RestoreHighLightMapping();
		}

	if (State == FSState::SetAnchor)
		if (SkylineUI->SetAnchorMapping->IsHighLight != Pressed) {
			if (Pressed) SkylineUI->SetHighLightMapping(SkylineUI->SetAnchorMapping);
			else SkylineUI->RestoreHighLightMapping();
		}

	if (State == FSState::SetItem)
		if (SkylineUI->SetCircleCenterMapping->IsHighLight != Pressed) {
			if (Pressed) SkylineUI->SetHighLightMapping(SkylineUI->SetCircleCenterMapping);
			else SkylineUI->RestoreHighLightMapping();
		}
}

void AFSController::SetRightMousePressed(bool Pressed)
{
	RightMousePressed = Pressed;
	if (SkylineUI->ExitMapping->IsHighLight != Pressed) {
		if (Pressed) SkylineUI->SetHighLightMapping(SkylineUI->ExitMapping);
		else SkylineUI->RestoreHighLightMapping();
	}
}

void AFSController::Tick(float dt)
{

	//this->BuildableSubsystem = AFGBuildableSubsystem::Get(this);


	//AFSkyline* FSkylin = AFSkyline::Get(this);
	//FSkylin->Select->UpdateHighlightedActors();

	if (!this->World) return;

	/*
	UFGHealthComponent* HealthComponent = GetPlayer()->GetHealthComponent();
	if (HealthComponent) {
		
		//HealthComponent->Heal(HealthComponent->GetMaxHealth() - HealthComponent->GetCurrentHealth());
		
		HealthComponent->Heal(HealthComponent->GetMaxHealth());
		HealthComponent->mIsDead = 0;
		HealthComponent->mOnAdjustDamage.Empty();
		HealthComponent->mDamageTypeModifiers.Empty();
		//GetPlayer()->bCanBeDamaged = !Etc->GetBool("Godlike");
	}
	*/

	FSInput->Tick();
	
	if (SkylineUI->IsActive) {
		if (State == FSState::Select) {
			TickSelect(dt);
		}
		if (State == FSState::Copy && !CopyWaitingResult) {
			TickCopy(dt);
		}
		if (State == FSState::SetItem) {
			TickSetItem(dt);
		}
		if (SkylineUI->Logo->IsShow && !SkylineUI->GettingStart->AccquireMenuWidget()->GetParent() && SkylineUI->Logo->Time.GetTime() > 15.0) {
			SkylineUI->Logo->StartFadeAnimation();
		}

		if (State == FSState::SetAreaAnchor) {
			if (areaSelectPressed) {
				
				FVector CamLoc;
				FRotator CamRot;
				this->FGController->GetPlayerViewPoint(CamLoc, CamRot);

				float DistanceOut = 2000.0f;
				const FVector TraceStart = CamLoc;
				const FVector Direction = CamRot.Vector();
				const FVector TraceEnd = CamLoc + (CamRot.Vector() * DistanceOut);

				CurrentMousePosition = TraceEnd;

				// Calculate box dimensions
				//FVector BoxExtent = ((InitialMousePosition - CurrentMousePosition) / 2);
				FVector BoxExtent = ((InitialMousePosition - CurrentMousePosition))/2;
				FVector BoxCenter = BoxStartPosition + BoxExtent;

				FActorSpawnParameters* parameters = new FActorSpawnParameters();

				FTransform cubeTransform = FTransform();
				cubeTransform.SetTranslation(BoxStartPosition);




				if (this->Select->Cube != nullptr) {
					if (this->Select->Cube->IsValidLowLevel()) {
						this->Select->Cube->Destroy();
						this->Select->Cube = nullptr;
					}
				}

				this->Select->Cube = (AMyActor*)this->World->SpawnActorAbsolute(AMyActor::StaticClass(), cubeTransform, *parameters);
				FBoxSphereBounds baseBounds = this->Select->Cube->GetRootComponent()->Bounds;

				FVector CubeScale = (BoxExtent / 50.0f);

				this->Select->Cube->SetActorScale3D(CubeScale);

				int AdjustmenX = BoxExtent.X;
				int AdjustmenY = BoxExtent.Y;
				int AdjustmenZ = BoxExtent.Z;


				FTransform transform = this->Select->Cube->GetTransform();

				FVector translate = transform.GetTranslation();

				FVector newPoint = FVector(translate.X - AdjustmenX, translate.Y - AdjustmenY, translate.Z - AdjustmenZ);


				transform.SetTranslation(newPoint);
				this->Select->Cube->SetActorTransform(transform);


				//BoxStartPosition.X
				//int Adjustment = CubeScale.X * 50.f;

				//FTransform transform = this->Select->Cube->GetTransform();

				//FVector translate = transform.GetTranslation();

				//FVector newPoint = FVector(translate.X + Adjustment, translate.Y, translate.Z);


				//transform.SetTranslation(newPoint);
				//this->Select->Cube->SetActorTransform(transform);

				this->Select->Cube->SetActorEnableCollision(false);

				std::string  str;
				//str.append("\n");
				str.append("AREA SELECT LOGIC INITIALIZED!\n");
				std::wstring temp = std::wstring(str.begin(), str.end());
				LPCWSTR wideString = temp.c_str();
				OutputDebugStringW(wideString);

				TInlineComponentArray<UMeshComponent*> MeshComps(this->Select->Cube);
				for (const auto& MeshComponent : MeshComps) {
					int num = MeshComponent->GetNumMaterials();
					if (num) {
						for (int i = 0; i < num; i++) {
							MeshComponent->SetMaterial(i, this->Select->CubeMaterial);
						}
					}
				}


			}
		}

	}

	if (CopyWaitingResult) {
		if (!this->Builder->IsBuilding) {
			CopyWaitingResult = false;

			//TODO FIX
			SkylineUI->MessageBox->SetVisibility(ESlateVisibility::Hidden);

			this->onBuildFinish();
		} else {
			SkylineUI->Message->SetText(FText::FromString(FString::Printf(TEXT("Waiting for building %d / %d"), Builder->SyncBuild->GetCurrent(), Builder->SyncBuild->GetTotal())));
		}
	}
	
	
	if (GetPlayer()) {
		if (Etc->GetBool("IsFlying")) {
			EMovementMode MovementMode = GetPlayer()->GetCharacterMovement()->MovementMode;
			if (MovementMode != EMovementMode::MOVE_Custom) {
				if (MovementMode != EMovementMode::MOVE_Flying) {
					if (MovementMode == EMovementMode::MOVE_Falling) {
						Etc->SetBool("IsFlying", !Etc->GetBool("IsFlying"));
						CheckFlying();
					} else {
						CheckFlying();
					}
				} else {
					TickFly(dt);
				}
			}
		}

		if (Etc->GetBool("InfiniteAmmo")) {
			AFGWeapon* Weapon = Cast<AFGWeapon>(GetPlayer()->GetEquipmentInSlot(EEquipmentSlot::ES_ARMS));
			if (Weapon) {
				Weapon->mCurrentAmmoCount = Weapon->GetMagSize();
			}
		}
	}
	
	
	if (Etc->GetBool("DisableHighSpaceFog")) {
		auto* heightFog = this->WorldSettings->GetExponentialHeightFog();

		if (heightFog) {
			UExponentialHeightFogComponent* Fog = this->WorldSettings->GetExponentialHeightFog()->GetComponent();
			if (Fog->FogDensity > 0.02) Fog->FogDensity = 0.02;
			if (Fog->GetComponentLocation().Z > -1500.0 + 1.0) {
				if (Fog->FogHeightFalloff < 0.18) Fog->FogHeightFalloff = 0.18;
				Fog->SetWorldLocation(FVector(0.0, 0.0, -1500.0));
			}
		}
	}

	
	if (FGController->IsMoveInputIgnored() && !this->FSInput->IsKeyDown("LeftControlKey"))
		ExitRectangleSelectMode();
	

	if (TurboSprint && !this->FSInput->IsKeyDown("TurboSprintKey")) TurboSprint = false;

	//doesnt work crashes engine
	/*
	if (Collectgarbage) {
		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
		Collectgarbage = false;
	}
	*/

	//doesnt work crashes engine
	// Check if DoWork steps are fully completed before setting up garbage collection
	/*
	if (IsDoWorkComplete && !bGarbageCollectionScheduled)
	{
		// Schedule garbage collection with a short delay
		GetWorld()->GetTimerManager().SetTimer(GCTimerHandle, this, &AFSController::PerformGarbageCollection, 0.1f, false);
		bGarbageCollectionScheduled = true;  // Set a flag to avoid multiple GC schedules
		IsDoWorkComplete = false;
	}
	*/

	if (IsDoWorkComplete)
	{
		IsDoWorkComplete = false;

		// Queue the garbage collection to the end of the frame
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float DeltaTime) {
			CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

			// Reset the flag once garbage collection completes
			bGarbageCollectionScheduled = false;
			return false;  // Return false to ensure it only executes once
			}), 0.1f);  // Schedule it with a slight delay
	}

}

// Define the garbage collection callback function
void AFSController::PerformGarbageCollection()
{
	CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	bGarbageCollectionScheduled = false;  // Reset flag after GC is complete
}

void AFSController::TickSelect(float dt)
{
	if (SelectWaitingResult) {
		if (this->Select->ConnectSelectCheckReady()) {
			SelectWaitingResult = false;
		}
	}
	if (!SelectWaitingResult && !RectangleSelectMode) {
		FSHitResults FSHit = this->GetSelectHitResult();
		FHitResult Hit = FSHit.Hit;
		AFGBuildable* Building = this->AcquireBuildable(Hit);

		FSBuildable Buildable;
		Buildable.Abstract = FSHit.Abstract;
		Buildable.Handle = FSHit.Handle;
		Buildable.RuntimeData = FSHit.RuntimeData;
		Buildable.BuildableClass = FSHit.BuildableClass;
		Buildable.Transform = FSHit.InstanceTransform;
		Buildable.Buildable = Building;

		FSBuildableTest Buildable1(this->AcquireBuildable(Hit));

		//fgcheck(Buildable.RuntimeData);
		//fgcheck(Buildable.BuildableClass);

		if (Buildable != CurrentFocusBuilding) {
			if (CurrentFocusBuilding != BuildableEmpty) this->ClearFocusBuilding();

 			if (Buildable != BuildableEmpty) {

				/* This won't always return a valid result so disable this debug for now*/
				/*
				std::string  str = TCHAR_TO_UTF8(*Building->GetName());
				str.append("\n");
				str.append("FOUND HIT\n");
				std::wstring temp = std::wstring(str.begin(), str.end());
				LPCWSTR wideString = temp.c_str();
				*/
				//OutputDebugStringW(wideString);

				if (this->FSInput->IsKeyDown("LeftMouseKey")) {
					if (!this->FSInput->IsKeyDown("RightMouseKey")) {
						if (!ConnectSelectMode) {
							if (!this->Design->IsElementSelected(Buildable)) {

								this->Select->Select(Buildable);
								LeftMousePressed = false;
							}
						}
					}
				}
				else {
					this->SetFocusBuilding(Buildable);
				}
			}
		}
	}
}

void AFSController::TickSetItem(float dt)
{
	FSHitResults FSHit = this->GetSelectHitResult();
	FHitResult Hit = FSHit.Hit;
	AFGBuildable* Building = this->AcquireBuildable(Hit);

	FSBuildable Buildable;
	Buildable.Abstract = FSHit.Abstract;
	Buildable.Handle = FSHit.Handle;
	Buildable.RuntimeData = FSHit.RuntimeData;
	Buildable.BuildableClass = FSHit.BuildableClass;
	Buildable.Buildable = Building;

	if (Buildable != this->CurrentFocusBuilding) {
		if (CurrentFocusBuilding != BuildableEmpty) this->ClearFocusBuilding();
		if (Building) this->SetFocusBuilding(Buildable);
	}
}

void AFSController::TickCopy(float dt)
{
	if (!CopyConsole) {
		FSHitResults Hit = this->GetCopyHitResult();
		this->Builder->Update(Hit.Hit);
	}
	else {
		this->Builder->Update(&this->Repeat);
	}
}

void AFSController::TickFly(float dt)
{
	if (!GetPlayer()) return;

	FVector CamLoc;
	FRotator CamRot;
	this->FGController->GetPlayerViewPoint(CamLoc, CamRot);
	FQuat CamQuat = CamRot.Quaternion();

	float Speed = 2000.0f;
	if (this->FSInput->IsKeyDown("SprintKey")) Speed = 4000.0f;
	if (TurboSprint) Speed = 20000.0f;
	FVector Velocity = FVector::ZeroVector;

	if (!FGController->IsMoveInputIgnored()) {
		if (this->FSInput->IsKeyDown("ForwardKey")) Velocity += CamQuat.GetAxisX();
		if (this->FSInput->IsKeyDown("BackwardKey")) Velocity -= CamQuat.GetAxisX();
		if (this->FSInput->IsKeyDown("LeftKey")) Velocity -= CamQuat.GetAxisY();
		if (this->FSInput->IsKeyDown("RightKey")) Velocity += CamQuat.GetAxisY();
		if (this->FSInput->IsKeyDown("GoUpKey")) Velocity += FRotator::ZeroRotator.Quaternion().GetAxisZ();
		if (this->FSInput->IsKeyDown("GoDownKey")) Velocity -= FRotator::ZeroRotator.Quaternion().GetAxisZ();
	}

	Velocity.Normalize(1.0f);
	GetPlayer()->GetCharacterMovement()->Velocity = Velocity * Speed;
}

void AFSController::LoadDesign(UFSDesign* DesignParam)
{
	if (this->Design && this->Design != DesignParam) UnloadDesign(false);
	this->Design = DesignParam;
	Repeat.Ready = false;

	if (this->Select != nullptr && this->Select->Cube != nullptr) {
		if (this->Select->Cube->IsValidLowLevel() && !this->Select->Cube->IsPendingKill()) {
			this->Select->Cube->Destroy();
			this->Select->Cube = nullptr;
		}
	}

}

void AFSController::UnloadDesign(bool ShowMouse)
{
	this->Design = nullptr;
	SetOpenState(true);
	if (ShowMouse) this->ShowMouseCursor();

	if (this->Select != nullptr && this->Select->Cube != nullptr) {
		if (this->Select->Cube->IsValidLowLevel() && !this->Select->Cube->IsPendingKill()) {
			this->Select->Cube->Destroy();
			this->Select->Cube = nullptr;
		}
	}

}

void AFSController::onCallMenu() {
	ExecCallMenu();
}

void AFSController::onEscPressed()
{
	if (SkylineUI->IsActive) {
		SetOpenState(true);

		State = FSState::Close;
		SkylineUI->HideMenu();
	}

	if (this->Select != nullptr && this->Select->Cube != nullptr) {
		if (this->Select->Cube->IsValidLowLevel() && !this->Select->Cube->IsPendingKill()) {
			this->Select->Cube->Destroy();
			this->Select->Cube = nullptr;
		}
	}

}

void AFSController::onLeftCtrlPressed()
{
	LastLeftCtrlPressed = World->GetRealTimeSeconds();
	LeftCtrlPressed = true;
	if (SkylineUI->IsActive) {
		if (State == FSState::Select && !SelectWaitingResult) {
			StartRectangleSelectMode();
			SkylineUI->SetHighLightMapping(SkylineUI->PressCtrlMapping);
		}
		else if (State == FSState::SetItem || State == FSState::SetAnchor || State == FSState::Copy) {
			SkylineUI->SetHighLightMapping(SkylineUI->PressCtrlMapping);
		}
	}
}

void AFSController::onLeftCtrlPressedRepeat()
{
	if (RectangleSelectMode) {
		if (World->GetRealTimeSeconds() - LastLeftCtrlPressed >= ShortPressLeftCtrlTimeLimit)
			if (!SkylineUI->HoldCtrlMapping->IsHighLight)
				SkylineUI->SetHighLightMapping(SkylineUI->HoldCtrlMapping);
	}
	
	if (State == FSState::SetItem || State == FSState::SetAnchor || State == FSState::Copy) {
		if (World->GetRealTimeSeconds() - LastLeftCtrlPressed >= ShortPressLeftCtrlTimeLimit)
			SkylineUI->RestoreHighLightMapping();
	}
}

void AFSController::onLeftCtrlReleased()
{
	ExitRectangleSelectMode();
	if (!LeftCtrlPressed) return;
	LeftCtrlPressed = false;

	if (World->GetRealTimeSeconds() - LastLeftCtrlPressed < ShortPressLeftCtrlTimeLimit) {
		if (IsShowMouseCursor()) {
			HideMouseCursor();
		}
		else {
			ShowMouseCursor();
		}
	}
	SkylineUI->RestoreHighLightMapping();
}

void AFSController::onDoubleSpacePressed()
{
	if (Etc->GetBool("DoubleSpaceFlying")) {
		Etc->SetBool("IsFlying", !Etc->GetBool("IsFlying"));
		CheckFlying();
	}
}

void AFSController::onDoubleShiftPressed()
{
	if (Etc->GetBool("DoubleShiftSprint")) {
		TurboSprint = true;
	}
}

void AFSController::onMouseScrollUp()
{
	if (LeftCtrlPressed) {

	}
	else {
		if (SkylineUI->IsActive) {
			if (State == FSState::Select) {
				ChangeConnectSelectMode();
			}
		}
	}

	if (SkylineUI->IsActive) {
		if (State == FSState::Copy && !CopyWaitingResult && !CopyConsole) {
			this->Builder->ScrollUp();
		}
	}

	SetLeftMousePressed(false);
	SetRightMousePressed(false);
}

void AFSController::onMouseScrollDown()
{
	if (LeftCtrlPressed) {

	}
	else {
		if (SkylineUI->IsActive) {
			if (State == FSState::Select) {
				ChangeConnectSelectMode();
			}
		}
	}

	if (SkylineUI->IsActive) {
		if (State == FSState::Copy && !CopyWaitingResult && !CopyConsole) {
			this->Builder->ScrollDown();
		}
	}

	SetLeftMousePressed(false);
	SetRightMousePressed(false);
}

void AFSController::onLeftMouseDown()
{
	if (!SkylineUI->IsActive) return;

	SetLeftMousePressed(false);

	if (LeftCtrlPressed) {
	}
	else {
		if (!SelectWaitingResult) {
			if (State == FSState::Select) {
				if (this->FSInput->IsKeyDown("RightMouseKey")) {
					onSelectModeLeftRightMouseClickWithoutLeftCtrl();
					SkylineUI->SetHighLightMapping(SkylineUI->SelectAnchorMapping);
				}
				else {
					if (!ConnectSelectMode) {
						/*
						AFGBuildable* Building = CurrentFocusBuilding.Buildable;
						if (Building && !this->Design->IsElementSelected(CurrentFocusBuilding)) {
							FSBuildable Buildable;
							Buildable.Buildable = Building;
							this->Select->Select(Buildable);
							SkylineUI->SetHighLightMapping(SkylineUI->SelectMapping);
						}
						*/
						AFGBuildable* Building = CurrentFocusBuilding.Buildable;
						if (CurrentFocusBuilding != BuildableEmpty) {
							//if (!this->Design->IsElementSelected(CurrentFocusBuilding)) {
								FSBuildable Buildable;
								Buildable.Buildable = Building;
								this->Select->Select(CurrentFocusBuilding);
								SkylineUI->SetHighLightMapping(SkylineUI->SelectMapping);
							//}
						}
						else SetLeftMousePressed(true);
					}
					else SetLeftMousePressed(true);
				}
			}
			else SetLeftMousePressed(true);
		}
	}

	if (State == FSState::SetAreaAnchor) {


		// Get mouse position
		float MouseX;
		float MouseY;
		FHitResult Hit;
		//AFGBuildable* Building;

		if (!areaSelectPressed) {
			areaSelectPressed = true;

			this->FGController->GetMousePosition(MouseX, MouseY);

			Hit = this->GetCopyHitResultGeneric();
			//Building = this->AcquireBuildable(Hit);

			//if (Building) {

				// Set initial mouse position and box start position
				InitialMousePosition = Hit.Location;
				BoxStartPosition = Hit.Location;

			//}


			//this->localPlayer

		}
	}

	SetRightMousePressed(false);
}

void AFSController::onLeftMouseUp()
{
	if (!SkylineUI->IsActive) return;

	if (LeftCtrlPressed) {
	}
	else {
		if (LeftMousePressed) {
			if (State == FSState::Select && !SelectWaitingResult) {
				if (CurrentFocusBuilding != BuildableEmpty) {
					if (ConnectSelectMode) {
						if (this->Select->ConnectSelect(CurrentFocusBuilding.Buildable)) {
							SelectWaitingResult = true;
						}
					}
					else this->Select->Select(CurrentFocusBuilding);

					// TODO REWORK THIS
					
					if (Design->Anchor != BuildableEmpty && !Design->IsElementSelected(Design->Anchor)) {
						Design->Anchor = FSBuildable();
						CheckAnchor(false);
					}
					
				}
			}
			else if (State == FSState::SetAnchor) {
				FSHitResults Hit = this->GetCopyHitResult();
				AFGBuildable* Building = this->AcquireBuildable(Hit.Hit);
				BuildingPtr = Building;

				// TODO REWORK THIS
				
				FSBuildable Buildable;
				Buildable.Abstract = Hit.Abstract;
				Buildable.Handle = Hit.Handle;
				Buildable.RuntimeData = Hit.RuntimeData;
				Buildable.BuildableClass = Hit.BuildableClass;
				Buildable.Transform = Hit.InstanceTransform;
				Buildable.Buildable = Building;

				if (this->Design->IsElementSelected(Buildable)) {
					this->Design->Anchor = Buildable;
					if (CheckAnchor(true)) {
						SetOpenState(true);
						StartCopyMode();
					}
				}
				
				BuildingPtr = nullptr;
			}
			else if (State == FSState::SetItem) {
				if (CurrentFocusBuilding != BuildableEmpty) {
					SkylineUI->OperatingWidget->AdvancedCopyPanel->SetCircleFeedback();
				}
			}
			else if (State == FSState::Copy && !CopyWaitingResult && !CopyConsole) {
				if (CopyPosition) {
					this->onSetRepeat();
				}
				else if (this->Builder->Build(nullptr)) {
					CopyWaitingResult = true;

					//TODO FIX
					SkylineUI->MessageBox->SetVisibility(ESlateVisibility::Visible);

					//this->onBuildFinish();
				}
			}
			else if (State == FSState::SetAreaAnchor) {
				/*
				FHitResult Hit = this->GetCopyHitResult();
				AFGBuildable* Building = this->AcquireBuildable(Hit);
				//BuildingPtr = Building;

				if (Building) {
					this->Select->SpawnInitialAreaBox(Building);

				}
				*/
			}
		}
		SkylineUI->RestoreHighLightMapping();
	}

	if (State == FSState::SetAreaAnchor) {
		if (areaSelectPressed) {
			areaSelectPressed = false;
		}
	}

	SetLeftMousePressed(false);
	SetRightMousePressed(false);
}

void AFSController::onRightMouseDown()
{
	LastRightMouseDown = World->GetRealTimeSeconds();
	if (!SkylineUI->IsActive) return;

	if (LeftCtrlPressed) {
	}
	else {
		if (State == FSState::Select && !SelectWaitingResult && this->FSInput->IsKeyDown("LeftMouseKey")) {
			onSelectModeLeftRightMouseClickWithoutLeftCtrl();
			SkylineUI->SetHighLightMapping(SkylineUI->SelectAnchorMapping);
		}
		else SetRightMousePressed(true);
	}
	SetLeftMousePressed(false);
}

void AFSController::onRightMouseDownRepeat()
{
	if (!SkylineUI->IsActive) return;
	if (LeftCtrlPressed) {

	}
	else {
		if (RightMousePressed) {
			if (World->GetRealTimeSeconds() - LastRightMouseDown >= ShortPressMouseRightTimeLimit) {
				SetRightMousePressed(false);
			}
		}
	}
}

void AFSController::onRightMouseUp()
{
	if (!SkylineUI->IsActive) return;

	if (LeftCtrlPressed) {
	}
	else {
		if (RightMousePressed && (State == FSState::Select || State == FSState::SetAnchor || State == FSState::SetItem || State == FSState::Copy || State == FSState::SetAreaAnchor && !CopyConsole)) {
			
			if (World->GetRealTimeSeconds() - LastRightMouseDown < ShortPressMouseRightTimeLimit) {
				SetOpenState(true);
				ShowMouseCursor();
			}
		}
		SkylineUI->RestoreHighLightMapping();
	}
	SetLeftMousePressed(false);
	SetRightMousePressed(false);
}

void AFSController::onActionShortcut1()
{
	if (!SkylineUI->IsActive) return;

	if (State == FSState::Select) {
		SetOpenState(true);
		if (CheckAnchor(true)) StartCopyMode();
		else StartSetAnchorMode();
	}
	else if (State == FSState::SetAnchor) {
		SetOpenState(true);
		StartSelectMode();
	}
	else if (State == FSState::Copy && !BuildOne && !CopyPosition && !CopyConsole) {
		SetOpenState(true);
		StartSelectMode();
	}
}

void AFSController::onSelectModeLeftRightMouseClickWithoutLeftCtrl()
{
	if (CurrentFocusBuilding != BuildableEmpty) {
		if (Design->Anchor == CurrentFocusBuilding) {
			Design->Anchor = BuildableEmpty;
			this->Select->DisableHightLightFocus();
		}
		else {
			if (!Design->IsElementSelected(CurrentFocusBuilding)) {
				this->Select->Select(CurrentFocusBuilding);
			}
			if (Design->Anchor != BuildableEmpty) {
				//FSBuildable Buildable;
				//Buildable.Buildable = Design->Anchor.Get();
				this->Select->EnableHightLight(Design->Anchor, Select->SelectMaterial);
			}
			Design->Anchor = CurrentFocusBuilding;
			this->Select->EnableHightLightFocus(CurrentFocusBuilding);
		}
		CheckAnchor(false);
	}
}

void AFSController::onMouseMove(const FVector2D& MousePosition)
{
	if (RectangleSelecting) {
		FVector2D Start = SelectStart;
		FVector2D End = MousePosition;
		if (End.X < Start.X) Swap(End.X, Start.X);
		if (End.Y < Start.Y) Swap(End.Y, Start.Y);

		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		this->SkylineUI->SelectRect->PanelSlot->SetPosition(Start / Scale);
		this->SkylineUI->SelectRect->PanelSlot->SetSize((End - Start) / Scale);

		if (Select->RectSelectCheckReady()) {
			Select->RectSelectUpdate(MousePosition);
		}
	}
}

void AFSController::onDesignDelete()
{
	if (!SkylineUI->IsActive) return;
	UFSDesignMenu* DesignMenu = SkylineUI->FindLastDesignItem();

	if (DesignMenu != SkylineUI->DesignRoot) {
		UFSDesignMenu* Parent = Cast<UFSDesignMenu>(DesignMenu->ParentItem);
		if (Parent) {
			if (DesignMenu->AccquireEntry() == SkylineUI->CurrentHighLightButton)
				SkylineUI->CurrentHighLightButton = nullptr;

			if (Parent->Folded) DesignMenu->Unfold();

			DesignMenu->Unexpand();

			Parent->RemoveChild(DesignMenu);
		}
	}
}

void AFSController::onPreResourceSinkTick(AFGBuildableResourceSink* Sink)
{
	if (!RecycleMaterials || !Sink) return;
	UFGFactoryConnectionComponent* Input = Cast<UFGFactoryConnectionComponent>(Sink->GetComponentByClass(UFGFactoryConnectionComponent::StaticClass()));

	FScopeLock ScopeLock(&Mutex);

	FInventoryItem out_item;
	float out_OffsetBeyond;
	if (Input && Input->Factory_GrabOutput(out_item, out_OffsetBeyond)) {
		UE_LOG(LogSkyline, Verbose, TEXT("%s"), *Sink->GetFullName());
		UE_LOG(LogSkyline, Verbose, TEXT("%s"), *out_item.ItemClass->GetFullName());
		//ResourceSinkSubsystem->AddPoints_ThreadSafe(out_item.ItemClass);
		Inventory.AddResource(out_item.ItemClass, 1);
	}
	
	/*
	TArray< FInventoryItem > out_items;
	if (Input->Factory_PeekOutput(out_items)) {
		if (out_items.Num() > 0) {
			SML::Logging::info(*Sink->GetFullName());
			SML::Logging::info(*out_items[0].ItemClass->GetFullName());
			
			if (ResourceSinkSubsystem->GetResourceSinkPointsForItem(out_items[0].ItemClass) == 0) {
				FInventoryItem out_item;
				float out_OffsetBeyond;
				Input->Factory_GrabOutput(out_item, out_OffsetBeyond);
			}
			Inventory.AddResource(out_items[0].ItemClass, 1);
		}
	}*/
}

bool AFSController::IsShowMouseCursor()
{
	return FGController->bShowMouseCursor;
}

void AFSController::ShowMouseCursor()
{
	UFGVirtualCursorFunctionLibrary::EnableVirtualCursor(FGController);
	FGController->bShowMouseCursor = true;
}

void AFSController::HideMouseCursor()
{
	UFGVirtualCursorFunctionLibrary::DisableVirtualCursor(FGController);
	FGController->bShowMouseCursor = false;
}

void AFSController::onBuildFinish()
{
	if (State == FSState::Copy) {
		if (BuildRepeatCallBack) {
			SkylineUI->RepeatWidget->CallBack();
		}
		if (BuildOne) {
			ExitCopyMode(true);
			ShowMouseCursor();
			Repeat.Source = Builder->Source;
			Repeat.Target = Builder->Target;
			Repeat.Ready = true;
		}
	}
}

void AFSController::onSetRepeat()
{
	if (State == FSState::Copy && CopyPosition && Builder->LastShow && (!Repeat.Ready || Repeat.Source.Equals(Builder->Source))) {
		if (Repeat.Ready) {
			Repeat.NextRelative.SetLocation(Repeat.Target.InverseTransformPositionNoScale(Builder->Target.GetLocation()));
			Repeat.NextRelative.SetRotation(Repeat.Target.InverseTransformRotation(Builder->Target.GetRotation()));
			SkylineUI->OperatingWidget->AdvancedCopyPanel->SetTransform(Repeat.NextRelative);

			Cast<UWidget>(this->SkylineUI->OperatingHelper)->SetVisibility(ESlateVisibility::Hidden);

			SkylineUI->RepeatWidget->Load();
			SkylineUI->RepeatWidget->StartEnterAnimation();
			CopyPosition = false;
			CopyConsole = true;
		}
		else {
			Repeat.Source = Builder->Source;
			Repeat.Target = Builder->Target;
			Repeat.Ready = true;
			ExitCopyMode(true);
		}
		ShowMouseCursor();
	}
}

void AFSController::ExecCallMenu()
{
	//UExponentialHeightFogComponent* Fog = this->WorldSettings->GetExponentialHeightFog()->GetComponent();
	//SML::Logging::info(Fog->FogDensity, TEXT(" "), Fog->FogHeightFalloff, TEXT(" "), *Fog->GetComponentLocation().ToString());

	if (SkylineUI->IsActive) {
		if (State == FSState::Select || State == FSState::SetAnchor || State == FSState::SetItem || State == FSState::Copy || State == FSState::SetAreaAnchor) {
			if (State == FSState::Select || State == FSState::SetAnchor || State == FSState::SetItem || State == FSState::Copy) {
				SetOpenState(true);
			}
			ShowMouseCursor();
		}
		else {
			if (SkylineUI->GettingStart->AccquireMenuWidget()->GetParent()) {
				SkylineUI->UnexpandStart(true);
			}
			else {
				SkylineUI->ExpandStart();
				ShowMouseCursor();
			}
		}
	}
	else {
		SkylineUI->OpenMenu();
		State = FSState::Open;
		ShowMouseCursor();
	}

}

void AFSController::EquipBuilder() {

	GetPlayer()->SetOverrideEquipment(this->FGBuildGun);
}

void AFSController::UnequipBuilder() {
	AFGEquipment* Equipment = GetPlayer()->GetEquipmentInSlot(EEquipmentSlot::ES_ARMS);
	if (Equipment == this->FGBuildGun) {
		GetPlayer()->ClearOverrideEquipment(this->FGBuildGun);
	}

}

void AFSController::PopFGUI() {
	this->FGUI->PopAllWidgets();
}

void AFSController::CheckFlying()
{
	//SML::Logging::info(*GetPlayer()->GetFullName());
	if (!GetPlayer()) return;
	UCharacterMovementComponent* MovementComponent = GetPlayer()->GetCharacterMovement();
  	if (!MovementComponent) return;
	if (Etc->GetBool("IsFlying")) {
		MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
		MovementComponent->MaxFlySpeed = 20000.0f;
		MovementComponent->MaxAcceleration = 0.0f;
	}
	else {

		bool hoverPackUsage = false;
		if (MovementComponent->CustomMovementMode == 4 && MovementComponent->MovementMode == 6) {
			hoverPackUsage = true;
		}
		if (!hoverPackUsage) {
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
			MovementComponent->MaxAcceleration = 10000.0f;
		}

		// TODO should be good as far as fix I think

		/*
		bool hoverPackUsage = false;
		if (MovementComponent->CustomMovementMode == 4) {
			hoverPackUsage = true;
		}
		if (MovementComponent->MovementMode == 5) {
			hoverPackUsage = true;
		}
		if (!hoverPackUsage) {
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Falling);
			//MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
			MovementComponent->MaxAcceleration = 10000.0f;
		}
		*/
	}
}

bool AFSController::CheckAnchor(bool Warn)
{
	if (&this->Design->Anchor) {
		if (this->Design->Anchor != BuildableEmpty) {

			// TODO REWORK THIS
			
			if (this->Builder->CheckAnchor(this->Design)) {

				if (this->Design->Anchor.Buildable) {
					this->Design->SetItemFeedback->SetText(FText::FromString(this->Design->Anchor.Buildable->GetName()));
				}
				this->Design->SetItemFeedback->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)));
				this->Design->SetItemFeedback->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				return true;
			}
			else {
				this->Design->SetItemFeedback->SetText(FText::FromString(TEXT("Anchor Not Valid")));
				this->Design->SetItemFeedback->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)));
				this->Design->SetItemFeedback->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				return false;
			}
			
		}
	}
	if (Warn) {
		this->Design->SetItemFeedback->SetText(FText::FromString(TEXT("Anchor Not Found")));
		this->Design->SetItemFeedback->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)));
		this->Design->SetItemFeedback->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else {
		this->Design->SetItemFeedback->SetVisibility(ESlateVisibility::Collapsed);
	}
	return false;
}

AFGBuildable* AFSController::AcquireBuildable(const FHitResult& Hit)
{
	if (Hit.GetActor()) {
		AFGBuildable* Buildable = Cast<AFGBuildable>(Hit.GetActor());
		if (Buildable && !Buildable->IsPlayingBuildEffect()) return Buildable;
		return nullptr;
	}
	return nullptr;
}

FSHitResults AFSController::GetSelectHitResult()
{
	FSHitResults FSHit;

	if (IsShowMouseCursor()) {
		// TODO DO WE NEED THIS?
		//return GetMouseCursorHitResult(true);
	}

	FHitResult Hit;

	FHitResult Hit2;

	Hit = this->FGBuildGun->GetHitResult();
	if (AcquireBuildable(Hit)) {
		FSHit.Hit = Hit;
		return FSHit;
	}


	FVector CamLoc;
	FRotator CamRot;
	this->FGController->GetPlayerViewPoint(CamLoc, CamRot);

	float max = 1000.0f;
	const FVector TraceStart = CamLoc;
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * DistanceMax);

	FCollisionQueryParams TraceParams(FName(TEXT("TraceUsableActor")), true, GetPlayer());
	TraceParams.AddIgnoredActor(this->FGBuildGun);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

	if (this->World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams)) {
		if (AcquireBuildable(Hit)) {
			FSHit.Hit = Hit;
			return FSHit;
		}
	}


	AFSkyline* FSkyline = AFSkyline::Get(this);
	AAbstractInstanceManager* Manager = AAbstractInstanceManager::GetInstanceManager(FSkyline->World);

	AFGLightweightBuildableSubsystem* lightweightSubsystem;

	lightweightSubsystem = AFGLightweightBuildableSubsystem::Get(FSkyline->World);

	//FGLightweightBuildableSubsystem* lightweightSubsystem = Cast< AFGLightweightBuildableSubsystem >(Actor)

	// Because of how hitresults are resolved in the manager we create a dummy one that specifies the manager as the actor.
	// If we find ourselves making more than this specific case trace situation we might want a more robust way of resolving hits against LightweightBuildables
	FHitResult modifiedHitResult = FHitResult(Manager, Hit.GetComponent(), Hit.Location, Hit.Normal);
	modifiedHitResult.Item = Hit.Item;
	//FInstanceHandle Handle;

	if (WorldHologramHelper == nullptr) {
		WorldHologramHelper = (AHologramHelper*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AHologramHelper::StaticClass(), Manager->GetTransform(), *(new FActorSpawnParameters()));
	}

	if (Manager->ResolveHit(modifiedHitResult, Handle))
	{
		//buildableClass = *Handle.GetInstancedBasedOnClass();
		//runtimeData = lightweightSubsystem->GetRuntimeDataForBuildableClassAndHandleNearLocation(buildableClass, Handle, Hit.Location);

		FLightweightBuildableInstanceRef buildableRef;
		AFGLightweightBuildableSubsystem::ResolveLightweightInstance(Handle, buildableRef);
		buildableClass = buildableRef.GetBuildableClass();
		runtimeData = const_cast<FRuntimeBuildableInstanceData*>(buildableRef.ResolveBuildableInstanceData());

		if (runtimeData)
		{

			/* it was a test but doesnt work to spawn the temporary buildables
			lightweightSubsystem->AddInstanceConverterInstigator(10000, nullptr, runtimeData->Transform);

			if (this->World->LineTraceSingleByChannel(Hit2, TraceStart, TraceEnd, ECC_Visibility, TraceParams)) {
				if (AcquireBuildable(Hit2)) {
					FSHit.Hit = Hit;
					return FSHit;
				}
			}
			*/

			FSHit.Abstract = true;
			FSHit.Handle = Handle;

			FRuntimeBuildableInstanceData RuntimeData;
			RuntimeData.Transform = runtimeData->Transform;
			RuntimeData.CustomizationData = runtimeData->CustomizationData;
			RuntimeData.BuiltWithRecipe = runtimeData->BuiltWithRecipe;
			RuntimeData.BlueprintProxy = runtimeData->BlueprintProxy;
			RuntimeData.Handles = runtimeData->Handles;

			FSHit.RuntimeData = RuntimeData;
			FSHit.BuildableClass = buildableClass;

			FTransform InstanceTransform;
			Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform, true);

			FSHit.InstanceTransform = InstanceTransform;

			//return FSHit;

			UHierarchicalInstancedStaticMeshComponent* OriginalHISMC = Handle.GetInstanceComponent();

			if (CompCopy == nullptr) {
				/*
				CompCopy = DuplicateObject<UHierarchicalInstancedStaticMeshComponent>(OriginalHISMC, WorldHologramHelper);

				CompCopy->ClearInstances();

				// Attach the duplicated component to the actor
				CompCopy->AttachToComponent(WorldHologramHelper->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
				CompCopy->RegisterComponent();

				// Make sure to set other properties as needed
				CompCopy->SetVisibility(true);
				CompCopy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				*/


				CompCopy = NewObject<UHierarchicalInstancedStaticMeshComponent>(WorldHologramHelper);


				FTransform InstanceRelativeTransform = Handle.GetInstanceComponent()->GetRelativeTransform();
				const FTransform InstanceSpawnLocation = InstanceRelativeTransform * WorldHologramHelper->GetActorTransform();

				CompCopy->SetStaticMesh(Handle.GetInstanceComponent()->GetStaticMesh());

				FInstanceData instanceData = FInstanceData();
				instanceData.OverridenMaterials.Add(FSkyline->Select->SelectMaterial);

				CompCopy->OverrideMaterials = instanceData.OverridenMaterials;
				CompCopy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				CompCopy->SetGenerateOverlapEvents(false);
				CompCopy->SetMobility(Handle.GetInstanceComponent()->Mobility);
				CompCopy->AttachToComponent(WorldHologramHelper->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

				CompCopy->SetRelativeTransform(FTransform::Identity);
				CompCopy->SetVisibility(true);
				//comp2->NumCustomDataFloats = InstanceHandles[i]->GetInstanceComponent()->NumCustomDataFloats;
				CompCopy->RegisterComponent();



			}

			//FTransform InstanceTransform;
			//Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform);
			//CompCopy->AddInstance(InstanceTransform);

			FTransform InstanceTransform2;
			Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform2, true);


			// worth noting runtimeData transformation data is off in world position from world position returned by the components
			// meaning this isnt accurate data to work with for some reason, investigate more to root cause to this as it should be the same?
			//InstanceTransform = runtimeData->Transform;

			CompCopy->AddInstance(InstanceTransform2, true);

			FVector ComponentWorldLocation = CompCopy->GetComponentLocation();
			FTransform InstanceTransform3;
			InstanceTransform3.SetLocation(ComponentWorldLocation);

			FSHit.InstanceTransform = FSHit.RuntimeData.Transform;

			CompCopy->ClearInstances();

			// doesnt work
			//bool didSpawn = false;
//			tempData = lightweightSubsystem->FindOrSpawnBuildableForRuntimeData(runtimeData, Handle.GetHandleID(), didSpawn);

			/*
			tempActor = lightweightSubsystem->AddInstanceConverterInstigator(10000, nullptr, FSHit.RuntimeData.Transform);
			lightweightSubsystem->SetInstanceInstigatorLocation(tempActor, FSHit.RuntimeData.Transform.GetLocation());

			FInstanceConverterInstigator converter;
			converter.Instigator = tempActor;
			converter.InfluenceRadius = 10000;

			FInstanceToTemporaryBuildable temp;

			//bool worked = converter.TryAddInstigatedBuildable(&temp);

			//if (worked) {
				return FSHitResults();
			//}
			*/

			return FSHit;

			Handle.HideInstance(true);


		}
	}

	return FSHitResults();
}

FSHitResults AFSController::GetCopyHitResult()
{
	if (IsShowMouseCursor()) {
		// TODO DO WE NEED THIS?
		//return GetMouseCursorHitResult(true);
	}

	FSHitResults FSHit;

	FHitResult Hit;
	
	Hit = this->FGBuildGun->GetHitResult();
	if (AcquireBuildable(Hit)) {

		FSHit.Hit = Hit;
		return FSHit;
	}

	FVector CamLoc;
	FRotator CamRot;
	this->FGController->GetPlayerViewPoint(CamLoc, CamRot);

	float max = 1000.0f;
	const FVector TraceStart = CamLoc;
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * DistanceMax);

	FCollisionQueryParams TraceParams(FName(TEXT("TraceUsableActor")), true, GetPlayer());
	TraceParams.AddIgnoredActor(this->FGBuildGun);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

	if (this->World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams)) {
		if (AcquireBuildable(Hit)) {
			FSHit.Hit = Hit;
			return FSHit;
		}
	}

	AFSkyline* FSkyline = AFSkyline::Get(this);
	AAbstractInstanceManager* Manager = AAbstractInstanceManager::GetInstanceManager(FSkyline->World);

	AFGLightweightBuildableSubsystem* lightweightSubsystem;

	lightweightSubsystem = AFGLightweightBuildableSubsystem::Get(FSkyline->World);

	//FGLightweightBuildableSubsystem* lightweightSubsystem = Cast< AFGLightweightBuildableSubsystem >(Actor)

	// Because of how hitresults are resolved in the manager we create a dummy one that specifies the manager as the actor.
	// If we find ourselves making more than this specific case trace situation we might want a more robust way of resolving hits against LightweightBuildables
	FHitResult modifiedHitResult = FHitResult(Manager, Hit.GetComponent(), Hit.Location, Hit.Normal);
	modifiedHitResult.Item = Hit.Item;
	//FInstanceHandle Handle;

	if (WorldHologramHelper == nullptr) {
		//WorldHologramHelper = (AHologramHelper*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AHologramHelper::StaticClass(), Manager->GetTransform(), *(new FActorSpawnParameters()));
	}

	if (Manager->ResolveHit(modifiedHitResult, Handle))
	{

		//both of these methods are deprected now
		//buildableClass = *Handle.GetInstancedBasedOnClass();
		//deprected in recent update so use new methods to get the runtimedata
		//runtimeData = lightweightSubsystem->GetRuntimeDataForBuildableClassAndHandleNearLocation(buildableClass, Handle, Hit.Location);

		FLightweightBuildableInstanceRef buildableRef;
		AFGLightweightBuildableSubsystem::ResolveLightweightInstance(Handle, buildableRef);
		buildableClass = buildableRef.GetBuildableClass();
		runtimeData = const_cast<FRuntimeBuildableInstanceData*>(buildableRef.ResolveBuildableInstanceData());

		if (runtimeData)
		{

			/* it was a test but doesnt work to spawn the temporary buildables
			lightweightSubsystem->AddInstanceConverterInstigator(10000, nullptr, runtimeData->Transform);

			if (this->World->LineTraceSingleByChannel(Hit2, TraceStart, TraceEnd, ECC_Visibility, TraceParams)) {
				if (AcquireBuildable(Hit2)) {
					FSHit.Hit = Hit;
					return FSHit;
				}
			}
			*/

			FSHit.Abstract = true;
			FSHit.Handle = Handle;

			FRuntimeBuildableInstanceData RuntimeData;
			RuntimeData.Transform = runtimeData->Transform;
			RuntimeData.CustomizationData = runtimeData->CustomizationData;
			RuntimeData.BuiltWithRecipe = runtimeData->BuiltWithRecipe;
			RuntimeData.BlueprintProxy = runtimeData->BlueprintProxy;
			RuntimeData.Handles = runtimeData->Handles;

			FSHit.RuntimeData = RuntimeData;

			//FSHit.RuntimeData = runtimeData;
			FSHit.BuildableClass = buildableClass;
			FSHit.Hit = Hit;

			FTransform InstanceTransform;
			Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform, true);

			//FSHit.InstanceTransform = InstanceTransform;

			FSHit.InstanceTransform = FSHit.RuntimeData.Transform;

			return FSHit;

			UHierarchicalInstancedStaticMeshComponent* OriginalHISMC = Handle.GetInstanceComponent();

			if (CompCopy == nullptr) {
				/*
				CompCopy = DuplicateObject<UHierarchicalInstancedStaticMeshComponent>(OriginalHISMC, WorldHologramHelper);

				CompCopy->ClearInstances();

				// Attach the duplicated component to the actor
				CompCopy->AttachToComponent(WorldHologramHelper->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
				CompCopy->RegisterComponent();

				// Make sure to set other properties as needed
				CompCopy->SetVisibility(true);
				CompCopy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				*/


				CompCopy = NewObject<UHierarchicalInstancedStaticMeshComponent>(WorldHologramHelper);


				FTransform InstanceRelativeTransform = Handle.GetInstanceComponent()->GetRelativeTransform();
				const FTransform InstanceSpawnLocation = InstanceRelativeTransform * WorldHologramHelper->GetActorTransform();

				CompCopy->SetStaticMesh(Handle.GetInstanceComponent()->GetStaticMesh());

				FInstanceData instanceData = FInstanceData();
				instanceData.OverridenMaterials.Add(FSkyline->Select->SelectMaterial);

				CompCopy->OverrideMaterials = instanceData.OverridenMaterials;
				CompCopy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				CompCopy->SetGenerateOverlapEvents(false);
				CompCopy->SetMobility(Handle.GetInstanceComponent()->Mobility);
				CompCopy->AttachToComponent(WorldHologramHelper->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

				CompCopy->SetRelativeTransform(FTransform::Identity);
				CompCopy->SetVisibility(true);
				//comp2->NumCustomDataFloats = InstanceHandles[i]->GetInstanceComponent()->NumCustomDataFloats;
				CompCopy->RegisterComponent();



			}

			//FTransform InstanceTransform;
			//Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform);
			//CompCopy->AddInstance(InstanceTransform);

			FTransform InstanceTransform2;
			Handle.GetInstanceComponent()->GetInstanceTransform(Handle.GetHandleID(), InstanceTransform2, true);


			// worth noting runtimeData transformation data is off in world position from world position returned by the components
			// meaning this isnt accurate data to work with for some reason, investigate more to root cause to this as it should be the same?
			//InstanceTransform = runtimeData->Transform;

			CompCopy->AddInstance(InstanceTransform2, true);

			//bool didSpawn = false;
			//tempData = lightweightSubsystem->FindOrSpawnBuildableForRuntimeData(runtimeData, Handle.GetHandleID(), didSpawn);


			Handle.HideInstance(true);


		}
	}

	FSHit.Hit = Hit;

	return FSHit;
}
FHitResult AFSController::GetCopyHitResultGeneric()
{
	//if (IsShowMouseCursor()) return GetMouseCursorHitResult(true);

	FHitResult Hit;

	Hit = this->FGBuildGun->GetHitResult();
	if (Hit.GetActor()) return Hit;


	FVector CamLoc;
	FRotator CamRot;
	this->FGController->GetPlayerViewPoint(CamLoc, CamRot);

	float max = 1000.0f;
	const FVector TraceStart = CamLoc;
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * DistanceMax);

	FCollisionQueryParams TraceParams(FName(TEXT("TraceUsableActor")), true, GetPlayer());
	TraceParams.AddIgnoredActor(this->FGBuildGun);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

	if (this->World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams)) {
		if (Hit.GetActor()) {
			return Hit;
		}
	}

	return Hit;
}

FHitResult AFSController::GetMouseCursorHitResult(bool RequireBuildable)
{
	if (this->SkylineUI->IsMouseCursorOnUI) return FHitResult();

	FCollisionQueryParams TraceParams(FName(TEXT("TraceUsableActor")), true, GetPlayer());
	TraceParams.AddIgnoredActor(this->FGBuildGun);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

	float x, y;
	if (this->FGController->GetMousePosition(x, y)) {
		FVector2D MousePosition(x, y);

		if (HUD->GetHitBoxAtCoordinates(MousePosition, true)) return FHitResult();

		FVector WorldOrigin;
		FVector WorldDirection;

		// TODO WE NEED TO DO THIS LOGIC DIFFERENTLY

		if (UGameplayStatics::DeprojectScreenToWorld(this->FGController, MousePosition, WorldOrigin, WorldDirection))
		{
			FHitResult Hit;
			if (this->World->LineTraceSingleByChannel(Hit, WorldOrigin, WorldOrigin + WorldDirection * DistanceMax, ECC_Visibility, TraceParams)) {
				if (!RequireBuildable || AcquireBuildable(Hit)) {
					return Hit;
				}
			}
		}
	}

	return FHitResult();
}

void AFSController::SetFocusBuilding(FSBuildable Buildable)
{
	//this->GetPlayer()->GetOutline()->ShowOutline(Buildable, EOutlineColor::OC_HOLOGRAM);
	//this->GetPlayer()->GetOutline()->ShowDismantlePendingMaterial(Buildable);
	CurrentFocusBuilding = Buildable;
	this->Select->EnableHightLightFocus(Buildable);
}

void AFSController::ClearFocusBuilding()
{
	if (CurrentFocusBuilding != BuildableEmpty) {
		this->Select->DisableHightLightFocus();
	}
	FSBuildable Empty;
	CurrentFocusBuilding = Empty;
}

void AFSController::ChangeConnectSelectMode()
{
	if (ConnectSelectMode) {
		ConnectSelectMode = false;
		this->SkylineUI->SelectScrollMapping->SetTitle(FText::FromString(TEXT("Select One")));
	}
	else {
		ConnectSelectMode = true;
		this->SkylineUI->SelectScrollMapping->SetTitle(FText::FromString(TEXT("Select Connected")));
	}
}

void AFSController::onSwitchFlying()
{
	if (!Etc->GetBool("DoubleSpaceFlying")) {
		Etc->SetBool("IsFlying", false);
		CheckFlying();
	}
}

void AFSController::onSwitchFog()
{
	//this->WorldSettings->GetExponentialHeightFog()->SetActorHiddenInGame(Etc->GetBool("DisableFog"));

	//fgcheck(this);
	//fgcheck(this->WorldSettings);
	auto* heightFog = this->WorldSettings->GetExponentialHeightFog();
	if (heightFog) {
		//fgcheck(heightFog);
		//fgcheck(Etc);
		heightFog->SetActorHiddenInGame(Etc->GetBool("DisableFog"));
	}

}

void AFSController::onSwitchGodlike()
{
	UFGHealthComponent* HealthComponent = GetPlayer()->GetHealthComponent();
	
	HealthComponent->Heal(HealthComponent->GetMaxHealth() - HealthComponent->GetCurrentHealth());
	
	//HealthComponent->Heal(HealthComponent->GetMaxHealth());
	//HealthComponent->mIsDead = 0;
	//HealthComponent->mOnAdjustDamage.Empty();
	//HealthComponent->mDamageTypeModifiers.Empty();
	GetPlayer()->bCanBeDamaged = !Etc->GetBool("Godlike");
}

void AFSController::onSwitchThirdPersonView()
{

	// TODO DO WE NEED A THIRD PERSON VIEW MODE?
	/*
	if (Etc->GetBool("ThirdPersonView"))
		GetPlayer()->SetThirdPersonMode();
	else
		GetPlayer()->SetFirstPersonMode();
	*/
}

void AFSController::onSwitchRecycleMaterials()
{
	this->RecycleMaterials = Etc->GetBool("RecycleMaterials");
}

AFGCharacterPlayer* AFSController::GetPlayer()
{
	if (this->localPlayer) {
		return this->localPlayer;
	}

	
	if (CurrentPlayer.Get()) return CurrentPlayer.Get();

	for (TObjectIterator<AFGCharacterPlayer>It; It; ++It) {
		if (It->IsLocallyControlled()) {
			CurrentPlayer = *It;
			this->FGBuildGun = GetPlayer()->GetBuildGun();
			//onSwitchThirdPersonView();
			return *It;
		}
	}
	
	//SML::Logging::info(TEXT("AFSController::GetPlayer"));
	return nullptr;
}