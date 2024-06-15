#include "GothGirlVoxelSplineActor.h"
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

AGothGirlVoxelSplineActor::AGothGirlVoxelSplineActor()
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
	Mask |= (1 << static_cast<int32>(EVoxel32BitMask::Channel0));
	Generator.Class = UVDI_Capsule_Graph::StaticClass();
}

void AGothGirlVoxelSplineActor::AddItemToWorld(AVoxelWorld* World)
{
	//LOG_VOXEL(Log, TEXT("AGothGirlVoxelSplineActor::AddItemToWorld"));
	if (IsValid(World))
	{
		if (Generator.Class == UVDI_Sphere_Graph::StaticClass())
		{
			int Max = Spline->GetNumberOfSplinePoints();

			for (int i = 0; i < Max; i++)
			{
				FVector Pos = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
				Pos = World->GlobalToLocalFloatBP(Pos);
				FVoxelIntBox IntBox = UVoxelIntBoxLibrary::MakeIntBoxFromPoints(TArray<FVector>{Pos});

				FVoxelDataItemConstructionInfo VDICI;

				VDICI.Parameters = TArray<float>
				{
					(float)Pos.X,
					(float)Pos.Y,
					(float)Pos.Z,
					Radius
				};
				VDICI.Mask = Mask;
				VDICI.Bounds = IntBox.Extend(UKismetMathLibrary::FCeil(Multiplier * Radius));
				VDICI.Generator = World->GetGeneratorCache().MakeGeneratorInstance(Generator);

				World->PlaceableItemManager->AddDataItem(VDICI);
			}
		}
		else
		{
			int Max = Spline->GetNumberOfSplinePoints() - 2;

			//LOG_VOXEL(Log, TEXT("AGothGirlVoxelSplineActor::AddItemToWorld Max: %i"), Max);

			for (int i = 0; i <= Max; i++)
			{
				FVector Pos = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
				FVector Pos2 = Spline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);

				Pos = World->GlobalToLocalFloatBP(Pos);
				Pos2 = World->GlobalToLocalFloatBP(Pos2);

				FVoxelIntBox IntBox = UVoxelIntBoxLibrary::MakeIntBoxFromPoints(TArray<FVector>{Pos, Pos2});

				FVoxelDataItemConstructionInfo VDICI;

				VDICI.Parameters = TArray<float>
				{
					(float)Pos.X,
					(float)Pos.Y,
					(float)Pos.Z,
					(float)Pos2.X,
					(float)Pos2.Y,
					(float)Pos2.Z,
					Radius
				};
				VDICI.Mask = Mask;
				VDICI.Bounds = IntBox.Extend(UKismetMathLibrary::FCeil(Multiplier * Radius));
				VDICI.Generator = World->GetGeneratorCache().MakeGeneratorInstance(Generator);

				World->PlaceableItemManager->AddDataItem(VDICI);

			}
		}
	}
}