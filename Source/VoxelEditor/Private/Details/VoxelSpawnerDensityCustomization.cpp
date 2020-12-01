// Copyright 2020 Phyronnaz

#include "Details/VoxelSpawnerDensityCustomization.h"
#include "VoxelSpawners/VoxelSpawnerConfig.h"
#include "VoxelEditorDetailsIncludes.h"

#define GET_CHILD_PROPERTY(Class, Property) PropertyHandle->GetChildHandle(GET_MEMBER_NAME_STATIC(Class, Property)).ToSharedRef()

void FVoxelSpawnerDensityCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	void* Address = nullptr;
	if (!ensure(PropertyHandle->GetValueData(Address) == FPropertyAccess::Success) || !ensure(Address))
	{
		return;
	}
	FVoxelSpawnerDensity& Density = *static_cast<FVoxelSpawnerDensity*>(Address);
	
	GET_CHILD_PROPERTY(FVoxelSpawnerDensity, Type)->SetOnPropertyValueChanged(FVoxelEditorUtilities::MakeRefreshDelegate(CustomizationUtils));
	GET_CHILD_PROPERTY(FVoxelSpawnerDensity, bUseMainGenerator)->SetOnPropertyValueChanged(FVoxelEditorUtilities::MakeRefreshDelegate(CustomizationUtils));

	IDetailGroup* Group = &ChildBuilder.AddGroup(TEXT("Spawner Density Type"), PropertyHandle->GetPropertyDisplayName());
	Group->HeaderRow()
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		GET_CHILD_PROPERTY(FVoxelSpawnerDensity, Type)->CreatePropertyValueWidget()
	];

	switch (Density.Type)
	{
	default: ensure(false);
	case EVoxelSpawnerDensityType::Constant:
	{
		Group->AddPropertyRow(GET_CHILD_PROPERTY(FVoxelSpawnerDensity, Constant));
		break;
	}
	case EVoxelSpawnerDensityType::GeneratorOutput:
	{
		Group->AddPropertyRow(GET_CHILD_PROPERTY(FVoxelSpawnerDensity, bUseMainGenerator));
		if (!Density.bUseMainGenerator)
		{
			Group->AddPropertyRow(GET_CHILD_PROPERTY(FVoxelSpawnerDensity, CustomGenerator));
		}
		Group->AddPropertyRow(GET_CHILD_PROPERTY(FVoxelSpawnerDensity, GeneratorOutputName));
		break;
	}
	case EVoxelSpawnerDensityType::MaterialRGBA:
	{
		Group->AddPropertyRow(GET_CHILD_PROPERTY(FVoxelSpawnerDensity, RGBAChannel));
		break;
	}
	case EVoxelSpawnerDensityType::MaterialUVs:
	{
		Group->AddPropertyRow(GET_CHILD_PROPERTY(FVoxelSpawnerDensity, UVChannel));
		Group->AddPropertyRow(GET_CHILD_PROPERTY(FVoxelSpawnerDensity, UVAxis));
		break;
	}
	case EVoxelSpawnerDensityType::MaterialFiveWayBlend:
	{
		Group->AddPropertyRow(GET_CHILD_PROPERTY(FVoxelSpawnerDensity, FiveWayBlendChannel));
		break;
	}
	case EVoxelSpawnerDensityType::SingleIndex:
	{
		Group->AddPropertyRow(GET_CHILD_PROPERTY(FVoxelSpawnerDensity, SingleIndexChannels));
		break;
	}
	case EVoxelSpawnerDensityType::MultiIndex:
	{
		Group->AddPropertyRow(GET_CHILD_PROPERTY(FVoxelSpawnerDensity, MultiIndexChannels));
		break;
	}
	}
	Group->AddPropertyRow(GET_CHILD_PROPERTY(FVoxelSpawnerDensity, Transform));
}

#undef GET_CHILD_PROPERTY