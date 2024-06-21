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
	//Builder.AddMultiIndex(1, 0.1f);
	
	return Builder.Build();
}

TVoxelRange<v_flt> MaxRange(const TVoxelRange<v_flt>& Range1, const TVoxelRange<v_flt>& Range2)
{
	return TVoxelRange<v_flt>(
		FMath::Max(Range1.Min, Range2.Min),
		FMath::Max(Range1.Max, Range2.Max));
}

TVoxelRange<v_flt> MinRange(const TVoxelRange<v_flt>& Range1, const TVoxelRange<v_flt>& Range2)
{
	return TVoxelRange<v_flt>(
		FMath::Min(Range1.Min, Range2.Min),
		FMath::Min(Range1.Max, Range2.Max));
}

template<bool bSubtractItems>
TVoxelRange<v_flt> CombineRanges(const TVoxelRange<v_flt>& ValueRange, const TVoxelRange<v_flt>& DataItemRange, EVoxelDataItemCombineMode CombineMode)
{
	if (CombineMode == EVoxelDataItemCombineMode::Max)
	{
		if (bSubtractItems)
		{
			// For subtraction, we want the max of the two ranges
			return MaxRange(ValueRange, DataItemRange);
		}
		else
		{
			// For addition, we want the min of the two ranges
			return MinRange(ValueRange, DataItemRange);
		}
	}
	else if (CombineMode == EVoxelDataItemCombineMode::Min)
	{
		if (bSubtractItems)
		{
			// For subtraction, we want the min of the two ranges
			return MinRange(ValueRange, DataItemRange);
		}
		else
		{
			// For addition, we want the max of the two ranges
			return MaxRange(ValueRange, DataItemRange);
		}
	}

	// Default case, should not reach here
	return TVoxelRange<v_flt>::Infinite();
}

TVoxelRange<v_flt> FGothGirlGeneratorSimpleFlatInstance::GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const
{
	// Define the buffer value to extend the bounds
	const v_flt BufferValue = 100.0f;  // Adjust this value as needed

	// Extend the bounds by adding the buffer value
	const FVoxelIntBox ExtendedBounds(
		Bounds.Min - FIntVector(BufferValue),
		Bounds.Max + FIntVector(BufferValue)
	);

	// Noise is between -1 and 1
	const TVoxelRange<v_flt> Height = TVoxelRange<v_flt>(-1, 1) * NoiseHeight;

	// Z can go from min to max
	TVoxelRange<v_flt> Value = TVoxelRange<v_flt>(ExtendedBounds.Min.Z, ExtendedBounds.Max.Z) - Height;

	// Adjust for the scaling factor used in GetValueImpl
	Value *= 0.2f;

	// Define the X, Y, and Z ranges based on the extended bounds
	TVoxelRange<v_flt> XRange(ExtendedBounds.Min.X, ExtendedBounds.Max.X);
	TVoxelRange<v_flt> YRange(ExtendedBounds.Min.Y, ExtendedBounds.Max.Y);
	TVoxelRange<v_flt> ZRange(ExtendedBounds.Min.Z, ExtendedBounds.Max.Z);

	// Now adjust the range for each DataItemConfig
	if (Items.ItemHolder.GetDataItems().Num() > 0)
	{
		for (const auto& DataItemConfig : DataItemConfigs)
		{
			TVoxelRange<v_flt> SmoothnessRange = TVoxelRange<v_flt>(DataItemConfig.Smoothness);
			TVoxelRange<v_flt> DataItemRange;

			DataItemRange = FVoxelUtilities::CombineDataItemDistanceRange(
				Value,
				Items.ItemHolder,
				XRange,
				YRange,
				ZRange,
				SmoothnessRange,
				DataItemConfig.Mask,
				DataItemConfig.bSubtractItems ? EVoxelDataItemCombineMode::Max : EVoxelDataItemCombineMode::Min);

			// Combine the ranges using our custom CombineRanges function
			Value = DataItemConfig.bSubtractItems ? CombineRanges<true>(Value, DataItemRange, EVoxelDataItemCombineMode::Max) :
				CombineRanges<false>(Value, DataItemRange, EVoxelDataItemCombineMode::Min);
		}
	}

	return Value;
}

FVector FGothGirlGeneratorSimpleFlatInstance::GetUpVector(v_flt X, v_flt Y, v_flt Z) const
{
	// Used by spawners
	return FVector::UpVector;
}
