#include "GothGirlVoxelWholeActor.h"
#include "VoxelMinimal.h"
#include "VoxelWorld.h"
#include "VoxelIntBoxLibrary.h"
#include "VoxelPlaceableItems/VoxelPlaceableItemManager.h"
#include "VoxelGenerators/VoxelGeneratorCache.h"
#include "Examples/VDI_Capsule_Graph.h"
#include "Examples/VDI_Sphere_Graph.h"
#include "VoxelGenerators/VoxelGeneratorInstanceWrapper.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"

AGothGirlVoxelWholeActor::AGothGirlVoxelWholeActor()
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

	USceneComponent* DefaultSceneRoot = GetRootComponent();
	if (!DefaultSceneRoot)
	{
		DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot");
		SetRootComponent(DefaultSceneRoot);
	}
	Mask |= (1 << static_cast<int32>(EVoxel32BitMask::Channel0));
	Generator.Class = UVDI_Capsule_Graph::StaticClass();
}

void AGothGirlVoxelWholeActor::AddItemToWorld(AVoxelWorld* World)
{
	//LOG_VOXEL(Log, TEXT("AGothGirlVoxelWholeActor::AddItemToWorld"));
	if (IsValid(World))
	{
		FVector HalfHeight = GetActorUpVector() * (Height * 0.5f * Multiplier);
		FVector Top = GetActorLocation() + HalfHeight;
		FVector Bottom = GetActorLocation() - HalfHeight;

		Top = World->GlobalToLocalFloatBP(Top);
		Bottom = World->GlobalToLocalFloatBP(Bottom);

		FVoxelIntBox IntBox = UVoxelIntBoxLibrary::MakeIntBoxFromPoints(TArray<FVector>{Top, Bottom});

		FVoxelDataItemConstructionInfo VDICI;

		if (Generator.Class == UVDI_Sphere_Graph::StaticClass())
		{
			FVector Location = World->GlobalToLocalFloatBP(GetActorLocation());

			IntBox = UVoxelIntBoxLibrary::MakeIntBoxFromPoints(TArray<FVector>{Location});

			VDICI.Parameters = TArray<float>
			{
				(float)Location.X,
				(float)Location.Y,
				(float)Location.Z,
				Radius
			};
		}
		else
		{
			VDICI.Parameters = TArray<float>
			{
				(float)Top.X,
				(float)Top.Y,
				(float)Top.Z,
				(float)Bottom.X,
				(float)Bottom.Y,
				(float)Bottom.Z,
				Radius
			};
		}

		VDICI.Mask = Mask;
		VDICI.Bounds = IntBox.Extend(UKismetMathLibrary::FCeil(Multiplier * Radius));
		VDICI.Generator = World->GetGeneratorCache().MakeGeneratorInstance(Generator);

		World->PlaceableItemManager->AddDataItem(VDICI);
	}
}