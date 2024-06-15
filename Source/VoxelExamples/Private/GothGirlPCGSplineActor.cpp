#include "GothGirlPCGSplineActor.h"
#include "VoxelMinimal.h"
#include "VoxelWorld.h"
#include "VoxelIntBoxLibrary.h"
#include "VoxelPlaceableItems/VoxelPlaceableItemManager.h"
#include "VoxelGenerators/VoxelGeneratorCache.h"
#include "Examples/VDI_Capsule_Graph.h"
#include "Examples/VDI_Sphere_Graph.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"

#include "PCGGraph.h"

AGothGirlPCGSplineActor::AGothGirlPCGSplineActor()
{
#if WITH_EDITOR
	static bool bAddedDelegate = false;
	if (!bAddedDelegate)
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.AddLambda([](UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
			{
				if (!PropertyChangedEvent.Property ||
					PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
				{
					return;
				}
				auto* Component = Cast<UActorComponent>(Object);
				if (!Component)
				{
					return;
				}
				auto* DataItemActor = Cast<AVoxelDataItemActor>(Component->GetOwner());
				if (!DataItemActor)
				{
					return;
				}

				DataItemActor->ScheduleRefresh();
			});
	}
#endif
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Spline = CreateDefaultSubobject<USplineComponent>(FName(TEXT("SplineComp")));
	SetRootComponent(Spline);
	Spline->SetClosedLoop(true);

	PCG = CreateDefaultSubobject<UPCGComponent>(FName(TEXT("PCGComp")));
	PCG->SetGraph(LoadObject<UPCGGraphInterface>(this, TEXT("/Game/FloatingIsland/Maps/Dungeons/Voxel/PCG/Graph/PCG_GothGirlVoxel_Spline")));
	PCG->Seed = 1337;
}


AGothGirlPCGExcluder::AGothGirlPCGExcluder()
	: Super()
{

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetStaticMeshComponent()->SetStaticMesh(LoadObject<UStaticMesh>(this, TEXT("/Engine/BasicShapes/Cube")));
	GetStaticMeshComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetStaticMeshComponent()->SetHiddenInGame(true);
	GetStaticMeshComponent()->SetHiddenInSceneCapture(true);
	GetStaticMeshComponent()->SetMaterial(0, LoadObject<UMaterialInterface>(this, TEXT("/Engine/BasicShapes/BasicShapeMaterial")));
	Tags.Add(FName(TEXT("PCG_EXCLUDE")));
}
