// Copyright Voxel Plugin SAS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/UObjectGlobals.h"

namespace FVoxelExampleUtilities
{
	template<typename T>
	T* LoadExampleObject(const TCHAR* Name)
	{
		return LoadObject<T>(nullptr, Name, nullptr, LOAD_NoWarn);
	}
}