#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelPlaceableItems/Actors/VoxelDataItemActor.h"
#include "Components/SplineComponent.h"
#include "PCGComponent.h"
#include "VoxelGenerators/VoxelGeneratorCache.h"
#include "Engine/StaticMeshActor.h"
#include "GothGirlPCGSplineActor.generated.h"

class USplineComponent;
class UPCGComponent;

UCLASS(ClassGroup = (Voxel), BlueprintType, Blueprintable)
class VOXELEXAMPLES_API AGothGirlPCGSplineActor : public AActor
{
	GENERATED_BODY()

public:
	AGothGirlPCGSplineActor();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> Spline = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPCGComponent> PCG = nullptr;

};

UCLASS(ClassGroup = (Voxel), BlueprintType, Blueprintable)
class VOXELEXAMPLES_API AGothGirlPCGExcluder : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AGothGirlPCGExcluder();
};
