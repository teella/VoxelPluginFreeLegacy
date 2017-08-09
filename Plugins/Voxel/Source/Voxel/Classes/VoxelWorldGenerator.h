// Copyright 2017 Phyronnaz

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VoxelWorldGenerator.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class VOXEL_API UVoxelWorldGenerator : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Get default value at position
	 * @param	Position		Position in voxel space
	 * @return	Value at position: between -127 and 127; positive if empty, negative if full
	 */
	virtual int GetDefaultValue(FIntVector Position);

	/**
	* Get default color at position
	* @param	Position		Position in voxel space
	* @return	Color at position
	*/
	virtual FColor GetDefaultColor(FIntVector Position);
};
