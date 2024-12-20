// Copyright Voxel Plugin SAS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VoxelGraphDataItemConfig.generated.h"

UCLASS()
class VOXELGRAPH_API UVoxelGraphDataItemConfig : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config")
	TArray<FName> Parameters;
};