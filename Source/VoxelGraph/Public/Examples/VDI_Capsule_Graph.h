// Copyright Voxel Plugin SAS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VoxelGeneratedWorldGeneratorsIncludes.h"
#include "VDI_Capsule_Graph.generated.h"

UCLASS(Blueprintable)
class VOXELGRAPH_API UVDI_Capsule_Graph : public UVoxelGraphGeneratorHelper
{
	GENERATED_BODY()
	
public:
	// Relative to the radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="", meta=(DisplayName="Noise Amplitude", UIMax="2", UIMin="0"))
	float Noise_Amplitude = 0.0f;
	// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="", meta=(DisplayName="Seed"))
	int32 Seed = 1337;
	
	UVDI_Capsule_Graph();
	virtual TVoxelSharedRef<FVoxelTransformableGeneratorInstance> GetTransformableInstance() override;
};
