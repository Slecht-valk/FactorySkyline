// This file has been automatically generated by the Unreal Header Implementation tool

#include "FGPipeBuilderTrail.h"

AFGPipeBuilderTrail::AFGPipeBuilderTrail() : Super() {
	this->mSpeed = 5;
	this->PrimaryActorTick.TickGroup = TG_PrePhysics; this->PrimaryActorTick.EndTickGroup = TG_PrePhysics; this->PrimaryActorTick.bTickEvenWhenPaused = false; this->PrimaryActorTick.bCanEverTick = true; this->PrimaryActorTick.bStartWithTickEnabled = true; this->PrimaryActorTick.bAllowTickOnDedicatedServer = true; this->PrimaryActorTick.TickInterval = 0;
	this->bReplicates = true;
}
void AFGPipeBuilderTrail::AddPipeline( AFGBuildablePipeline* inPipe){ }
void AFGPipeBuilderTrail::StartSplineMovement(){ }
void AFGPipeBuilderTrail::PlayBuildEffect(){ }
void AFGPipeBuilderTrail::EffectDone(){ }
void AFGPipeBuilderTrail::BeginPlay(){ }
void AFGPipeBuilderTrail::Tick(float DeltaTime){ }