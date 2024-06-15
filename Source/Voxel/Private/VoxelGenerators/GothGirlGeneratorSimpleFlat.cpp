#include "VoxelGenerators/GothGirlGeneratorSimpleFlat.h"
#include "FastNoise/VoxelFastNoise.inl"
#include "VoxelMaterialBuilder.h"
#include "VoxelGenerators/VoxelGeneratorHelpers.h"
#include "VoxelUtilities/VoxelDataItemUtilities.h"

TVoxelSharedRef<FVoxelGeneratorInstance> UGothGirlGeneratorSimpleFlat::GetInstance()
{
	return MakeVoxelShared<FGothGirlGeneratorSimpleFlatInstance>(*this);
}

///////////////////////////////////////////////////////////////////////////////

FGothGirlGeneratorSimpleFlatInstance::FGothGirlGeneratorSimpleFlatInstance(const UGothGirlGeneratorSimpleFlat& MyGenerator)
	: Super(&MyGenerator)
	, DataItemConfigs(MyGenerator.DataItemConfigs)
	, NoiseHeight(MyGenerator.NoiseHeight)
	, Frequence(MyGenerator.Frequence)
	, Seed(MyGenerator.Seed)
{
}

void FGothGirlGeneratorSimpleFlatInstance::Init(const FVoxelGeneratorInit& InitStruct)
{
	Noise.SetSeed(Seed);
}

v_flt FGothGirlGeneratorSimpleFlatInstance::GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	v_flt Value = Z + 0.001f; // Try to avoid having 0 as density, as it behaves weirdly

	if (NoiseHeight > 0.0f)
	{
		const float Height = Noise.GetPerlin_2D(X, Y, Frequence) * NoiseHeight;

		// Positive value -> empty voxel
		// Negative value -> full voxel
		// Value positive when Z > Height, and negative Z < Height
		Value = Z - Height;

		// The voxel value is clamped between -1 and 1. That can result in a bad gradient/normal. To solve that we divide it
		//Value /= 5;
		Value *= 0.2f;
	}

	if (Items.ItemHolder.GetDataItems().Num() > 0)
	{
		for (auto& DataItemConfig : DataItemConfigs)
		{
			if (DataItemConfig.bSubtractItems)
			{
				Value = FVoxelUtilities::CombineDataItemDistance<true>(Value, Items.ItemHolder, X, Y, Z, DataItemConfig.Smoothness, DataItemConfig.Mask, EVoxelDataItemCombineMode::Max);
			}
			else
			{
				Value = FVoxelUtilities::CombineDataItemDistance<false>(Value, Items.ItemHolder, X, Y, Z, DataItemConfig.Smoothness, DataItemConfig.Mask, EVoxelDataItemCombineMode::Min);
			}
		}
	}

	return Value;
}

FVoxelMaterial FGothGirlGeneratorSimpleFlatInstance::GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	FVoxelMaterialBuilder Builder;

	// RGB
	//Builder.SetMaterialConfig(EVoxelMaterialConfig::RGB);
	//Builder.SetColor(FColor::Red);

	// Single index
	//Builder.SetMaterialConfig(EVoxelMaterialConfig::SingleIndex);
	//Builder.SetSingleIndex(0); 

	// Multi index
	Builder.SetMaterialConfig(EVoxelMaterialConfig::MultiIndex);
	Builder.AddMultiIndex(0, 0.5f);
	Builder.AddMultiIndex(1, 0.5f);
	
	return Builder.Build();
}

TVoxelRange<v_flt> FGothGirlGeneratorSimpleFlatInstance::GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const
{
	// Return the values that GetValueImpl can return in Bounds
	// Used to skip chunks where the value does not change
	// Be careful, if wrong your world will have holes!
	// By default return infinite range to be safe
	//if (NoiseHeight <= 0.0f)
	//{
		return TVoxelRange<v_flt>::Infinite();
	//}
	/*
	* This is good for optimization, makes the voxel system tight around the generated area.
	* but you can't dig under it
	// Example for the GetValueImpl above

	// Noise is between -1 and 1
	const TVoxelRange<v_flt> Height = TVoxelRange<v_flt>(-1, 1) * NoiseHeight;

	// Z can go from min to max
	TVoxelRange<v_flt> Value = TVoxelRange<v_flt>(Bounds.Min.Z, Bounds.Max.Z) - Height;

	//Value /= 5;
	Value *= 0.2f;

	return Value;
	*/
}

FVector FGothGirlGeneratorSimpleFlatInstance::GetUpVector(v_flt X, v_flt Y, v_flt Z) const
{
	// Used by spawners
	return FVector::UpVector;
}
