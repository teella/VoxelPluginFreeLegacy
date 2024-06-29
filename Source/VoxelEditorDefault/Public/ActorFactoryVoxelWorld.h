// Copyright Voxel Plugin SAS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActorFactories/ActorFactory.h"
#include "Engine/StaticMeshActor.h"
#include "ActorFactoryVoxelWorld.generated.h"

UCLASS()
class VOXELEDITORDEFAULT_API UActorFactoryVoxelWorld : public UActorFactory
{
	GENERATED_BODY()

public:
	UActorFactoryVoxelWorld();

	//~ Begin UActorFactory Interface
	void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	//~ End UActorFactory Interface
};

UCLASS()
class VOXELEDITORDEFAULT_API UActorFactoryGothGirlWhole : public UActorFactory
{
	GENERATED_BODY()

public:
	UActorFactoryGothGirlWhole();

	//~ Begin UActorFactory Interface
	void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	//~ End UActorFactory Interface
};


UCLASS()
class VOXELEDITORDEFAULT_API UActorFactoryGothGirlSpline : public UActorFactory
{
	GENERATED_BODY()

public:
	UActorFactoryGothGirlSpline();

	//~ Begin UActorFactory Interface
	void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	//~ End UActorFactory Interface
};

UCLASS()
class VOXELEDITORDEFAULT_API UActorFactoryGothGirlPCG : public UActorFactory
{
	GENERATED_BODY()

public:
	UActorFactoryGothGirlPCG();

	//~ Begin UActorFactory Interface
	void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	//~ End UActorFactory Interface
};

UCLASS()
class VOXELEDITORDEFAULT_API UActorFactoryGothGirlPCGExclude : public UActorFactory
{
	GENERATED_BODY()

public:
	UActorFactoryGothGirlPCGExclude();

	//~ Begin UActorFactory Interface
	void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	//~ End UActorFactory Interface
};

UCLASS()
class VOXELEDITORDEFAULT_API UActorFactoryGothGirlSplineRoad : public UActorFactory
{
	GENERATED_BODY()

public:
	UActorFactoryGothGirlSplineRoad();

	//~ Begin UActorFactory Interface
	void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	//~ End UActorFactory Interface
};
