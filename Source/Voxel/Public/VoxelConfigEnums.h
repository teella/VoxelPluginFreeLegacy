// Copyright 2020 Phyronnaz

#pragma once

#include "CoreMinimal.h"
#include "VoxelConfigEnums.generated.h"

UENUM(BlueprintType)
enum class EVoxelRenderType : uint8
{
	MarchingCubes,
	Cubic,
	// Surface nets only work well at LOD 0. They will have holes between higher LODs, and the material won't be picked correctly.
	SurfaceNets
};

UENUM(BlueprintType)
enum class EVoxelNormalConfig : uint8
{
	NoNormal = 0,
	// Use the density field gradient as normal. Might have glitches on hard corners which can be quite visible when using triplanar projection
	GradientNormal = 1,
	// Compute the normal from the mesh faces. This will have glitches on chunks borders, Gradient Normal are preferred
	MeshNormal = 2
};

UENUM(BlueprintType)
enum class EVoxelMaterialConfig : uint8
{
	RGB,
	SingleIndex,
	DoubleIndex_DEPRECATED UMETA(Hidden),
	MultiIndex
};

UENUM(BlueprintType)
enum class EVoxelUVConfig : uint8
{
	GlobalUVs			UMETA(DisplayName = "Global UVs"),
	PackWorldUpInUVs	UMETA(DisplayName = "Pack WorldUp in UVs"),
	// In Cubic, per Voxel. In others, per chunk
	PerVoxelUVs			UMETA(DisplayName = "Per Voxel/Chunk UVs"),
	Max					UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EVoxelRGBA : uint8
{
	R,
	G,
	B,
	A
};

UENUM(BlueprintType)
enum class EVoxelSpawnerActorSpawnType : uint8
{
	// Spawn all spawner actors
	All,
	// Spawn only floating spawner actors
	OnlyFloating
};

UENUM(BlueprintType)
enum class EVoxelSamplerMode : uint8
{
	// Clamp the coordinates
	Clamp,
	// Tile the coordinates
	Tile
};

enum class EVoxelPlayType
{
	Game,
	Preview
};

UENUM(BlueprintType)
enum class EVoxelDataType : uint8
{
	Values,
	Materials
};

UENUM(BlueprintType)
enum class EVoxelRGBHardness : uint8
{
	// Interpret the material as 4 way blend, and use MaterialsHardness
	FourWayBlend,
	// Interpret the material as 5 way blend, and use MaterialsHardness
	FiveWayBlend,
	// Use the Red channel as hardness
	R,
	// Use the Green channel as hardness
	G,
	// Use the Blue channel as hardness
	B,
	// Use the Alpha channel as hardness
	A,
	// Use the U0 channel as hardness
	U0,
	// Use the U1 channel as hardness
	U1,
	// Use the V0 channel as hardness
	V0,
	// Use the V1 channel as hardness
	V1
};

UENUM(BlueprintType)
enum class EVoxelFalloff : uint8
{
	Linear,
	Smooth,
	Spherical,
	Tip
};

UENUM(BlueprintType)
enum class EVoxelComputeDevice : uint8
{
	CPU,
	GPU
};

UENUM(BlueprintType)
enum class EVoxelAxis : uint8
{
	X,
	Y,
	Z
};