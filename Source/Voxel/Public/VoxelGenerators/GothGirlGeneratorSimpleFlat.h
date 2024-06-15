
#pragma once

#include "CoreMinimal.h"
#include "FastNoise/VoxelFastNoise.h"
#include "VoxelGenerators/VoxelGeneratorHelpers.h"
#include "GothGirlGeneratorSimpleFlat.generated.h"

class FGothGirlGeneratorSimpleFlatInstance;

USTRUCT(BlueprintType, Blueprintable)
struct VOXEL_API FGothGirlFlatGeneratorDataItemConfig
{
	GENERATED_BODY()

public:
	// In voxels, how smooth the intersection with the existing terrain and these items should be
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float Smoothness = 10;

	// Only items matching this mask will be added
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (Bitmask, BitmaskEnum = "/Script/Voxel.EVoxel32BitMask"))
	int32 Mask = 0;

	// If true, will subtract the items from the world and will invert their values
	// If false, will add the items to the world and will not invert their values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool bSubtractItems = false;
};

UCLASS(Blueprintable)
class VOXEL_API UGothGirlGeneratorSimpleFlat : public UVoxelGenerator
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	float NoiseHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	float Frequence = 0.01f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	int32 Seed = 1337;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	TArray<FGothGirlFlatGeneratorDataItemConfig> DataItemConfigs =
	{
		{
			2,
			1 << 0,
			true
		},
		{
			2,
			1 << 1,
			false
		},
	};

	//~ Begin UVoxelGenerator Interface
	virtual TVoxelSharedRef<FVoxelGeneratorInstance> GetInstance() override;
	//~ End UVoxelGenerator Interface
};

class FGothGirlGeneratorSimpleFlatInstance : public TVoxelGeneratorInstanceHelper<FGothGirlGeneratorSimpleFlatInstance, UGothGirlGeneratorSimpleFlat>
{
public:
    using Super = TVoxelGeneratorInstanceHelper<FGothGirlGeneratorSimpleFlatInstance, UGothGirlGeneratorSimpleFlat>;

	explicit FGothGirlGeneratorSimpleFlatInstance(const UGothGirlGeneratorSimpleFlat& MyGenerator);

	const TArray<FGothGirlFlatGeneratorDataItemConfig> DataItemConfigs;

	//~ Begin FVoxelGeneratorInstance Interface
	virtual void Init(const FVoxelGeneratorInit& InitStruct) override;

	v_flt GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;
	FVoxelMaterial GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;

	TVoxelRange<v_flt> GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const;

	virtual FVector GetUpVector(v_flt X, v_flt Y, v_flt Z) const override final;
	//~ End FVoxelGeneratorInstance Interface

private:
	const float NoiseHeight;
	const float Frequence;
	const int32 Seed;
	FVoxelFastNoise Noise;
};
