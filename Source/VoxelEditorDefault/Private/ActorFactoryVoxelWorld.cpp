// Copyright Voxel Plugin SAS. All Rights Reserved.

#include "ActorFactoryVoxelWorld.h"
#include "VoxelWorld.h"
#include "VoxelUtilities/VoxelExampleUtilities.h"
#include "VoxelGenerators/GothGirlGeneratorSimpleFlat.h"
#include "VoxelRender/MaterialCollections/VoxelMaterialCollectionBase.h"

#include "Materials/MaterialInterface.h"

#include "GothGirlVoxelWholeActor.h"
#include "GothGirlVoxelSplineActor.h"
#include "GothGirlPCGSplineActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"

UActorFactoryVoxelWorld::UActorFactoryVoxelWorld()
{
	DisplayName = VOXEL_LOCTEXT("Voxel World");
	NewActorClass = AVoxelWorld::StaticClass();
}

void UActorFactoryVoxelWorld::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

	AVoxelWorld* VoxelWorld = CastChecked<AVoxelWorld>(NewActor);
	VoxelWorld->bCreateWorldAutomatically = true;
	VoxelWorld->bUseCameraIfNoInvokersFound = true;
	VoxelWorld->SetGeneratorClass(UGothGirlGeneratorSimpleFlat::StaticClass());
	VoxelWorld->MaterialConfig = EVoxelMaterialConfig::RGB;
	VoxelWorld->MaterialCollection = FVoxelExampleUtilities::LoadExampleObject<UVoxelMaterialCollectionBase>(TEXT("/Voxel/Examples/Materials/Quixel/MC_Quixel"));
	VoxelWorld->VoxelMaterial = FVoxelExampleUtilities::LoadExampleObject<UMaterialInterface>(TEXT("/Voxel/Examples/Materials/Quixel/MI_VoxelQuixel_FiveWayBlend_Inst"));
	if (!VoxelWorld->IsCreated())
	{
		VoxelWorld->Toggle();
	}
}

UActorFactoryGothGirlWhole::UActorFactoryGothGirlWhole()
{
	DisplayName = VOXEL_LOCTEXT("GothGirl Voxel Whole");
	NewActorClass = AVoxelWorld::StaticClass();
}

void UActorFactoryGothGirlWhole::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

	//AGothGirlVoxelWholeActor* VoxelWorld = CastChecked<AGothGirlVoxelWholeActor>(NewActor);
}

UActorFactoryGothGirlSpline::UActorFactoryGothGirlSpline()
{
	DisplayName = VOXEL_LOCTEXT("GothGirl Voxel Spline");
	NewActorClass = AVoxelWorld::StaticClass();
}

void UActorFactoryGothGirlSpline::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

	//AGothGirlVoxelSplineActor* VoxelWorld = CastChecked<AGothGirlVoxelSplineActor>(NewActor);
}

UActorFactoryGothGirlPCG::UActorFactoryGothGirlPCG()
{
	DisplayName = VOXEL_LOCTEXT("GothGirl PCG Spline");
	NewActorClass = AVoxelWorld::StaticClass();
}

void UActorFactoryGothGirlPCG::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

	//AGothGirlVoxelSplineActor* VoxelWorld = CastChecked<AGothGirlVoxelSplineActor>(NewActor);
}

UActorFactoryGothGirlPCGExclude::UActorFactoryGothGirlPCGExclude()
{
	DisplayName = VOXEL_LOCTEXT("GothGirl PCG_EXCLUDE");
	NewActorClass = AVoxelWorld::StaticClass();
}

void UActorFactoryGothGirlPCGExclude::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

}
