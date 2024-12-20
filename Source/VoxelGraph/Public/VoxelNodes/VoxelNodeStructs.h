// Copyright Voxel Plugin SAS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VoxelPinCategory.h"
#include "VoxelNodeStructs.generated.h"

USTRUCT()
struct FVoxelNamedDataPin
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Voxel")
	EVoxelDataPinCategory Type = EVoxelDataPinCategory::Float;

	UPROPERTY(EditAnywhere, Category = "Voxel")
	FString Name = "Value";
};