#pragma once

#include "CoreMinimal.h"
#include "VoxelGeneratedWorldGeneratorsIncludes.h"
#include "GothGirlGenerator_Ravines.generated.h"

UCLASS(Blueprintable)
class UGothGirlGenerator_Ravines : public UVoxelGraphGeneratorHelper
{
	GENERATED_BODY()
	
public:
	// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="", meta=(DisplayName=" 3D Noise Frequency", UIMax="1", UIMin="0"))
	float _3D_Noise_Frequency = 0.03;
	// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="", meta=(DisplayName="3D Noise Seed"))
	int32 _3D_Noise_Seed = 1337;
	// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="", meta=(DisplayName="Bottom Transition Smoothness"))
	float Bottom_Transition_Smoothness = 10.0;
	// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="", meta=(DisplayName="Height"))
	float Height = 50.0;
	// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="", meta=(DisplayName="Top Transition Smoothness"))
	float Top_Transition_Smoothness = 10.0;
	
	UGothGirlGenerator_Ravines();
	virtual TVoxelSharedRef<FVoxelTransformableGeneratorInstance> GetTransformableInstance() override;
};
