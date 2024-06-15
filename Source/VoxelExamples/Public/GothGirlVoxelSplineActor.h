#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelPlaceableItems/Actors/VoxelDataItemActor.h"
#include "Components/SplineComponent.h"
#include "VoxelGenerators/VoxelGeneratorCache.h"
#include "GothGirlVoxelSplineActor.generated.h"

class AVoxelWorld;

UCLASS(ClassGroup = (Voxel), BlueprintType, Blueprintable)
class VOXELEXAMPLES_API AGothGirlVoxelSplineActor : public AVoxelDataItemActor
{
	GENERATED_BODY()

public:
	AGothGirlVoxelSplineActor();

public:
	virtual void AddItemToWorld(AVoxelWorld* World) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> Spline = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Voxel, meta = (AllowPrivateAccess = "true"))
	FVoxelGeneratorPicker Generator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Voxel, meta = (AllowPrivateAccess = "true"))
	float Radius{ 10.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Voxel, meta = (AllowPrivateAccess = "true"))
	float Multiplier{ 2.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Voxel, meta = (AllowPrivateAccess = "true", Bitmask, BitmaskEnum = "/Script/Voxel.EVoxel32BitMask"))
	int32 Mask = 0;
};