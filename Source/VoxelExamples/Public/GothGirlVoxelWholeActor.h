#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelPlaceableItems/Actors/VoxelDataItemActor.h"
#include "Components/SplineComponent.h"
#include "VoxelGenerators/VoxelGeneratorCache.h"
#include "GothGirlVoxelWholeActor.generated.h"

class AVoxelWorld;

UCLASS(ClassGroup = (Voxel), BlueprintType, Blueprintable)
class VOXELEXAMPLES_API AGothGirlVoxelWholeActor : public AVoxelDataItemActor
{
	GENERATED_BODY()

public:
	AGothGirlVoxelWholeActor();

public:
	virtual void AddItemToWorld(AVoxelWorld* World) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Voxel, meta = (AllowPrivateAccess = "true"))
	FVoxelGeneratorPicker Generator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Voxel, meta = (AllowPrivateAccess = "true"))
	float Height{ 10.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Voxel, meta = (AllowPrivateAccess = "true"))
	float Radius{ 10.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Voxel, meta = (AllowPrivateAccess = "true"))
	float Multiplier{ 2.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Voxel, meta = (AllowPrivateAccess = "true", Bitmask, BitmaskEnum = "/Script/Voxel.EVoxel32BitMask"))
	int32 Mask = 0;
};