// Copyright 2017 Phyronnaz

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "TransitionDirection.h"
#include "Components/LineBatchComponent.h"
#include "VoxelChunk.generated.h"

class AVoxelWorld;
class AVoxelTransitionChunk;
class UProceduralMeshComponent;
class VoxelThread;

/**
 * Voxel Chunk actor class
 */
UCLASS()
class AVoxelChunk : public AActor
{
	GENERATED_BODY()

public:
	friend class VoxelThread;

	AVoxelChunk();

	/**
	 * Init this
	 * @param	NewPosition		Position of this (Minimal corner)
	 * @param	NewDepth		Width = 16 * 2^Depth
	 * @param	NewWorld		VoxelWorld
	 */
	void Init(FIntVector NewPosition, int NewDepth, AVoxelWorld* NewWorld);

	/**
	 * Update this for terrain changes
	 * @param	bAsync
	 */
	void Update(bool bAsync);

	/**
	 * Check if an adjacent chunk has changed its resolution, and update async if needed
	 */
	void BasicUpdate();

	/**
	 * Schedule destruction of this chunk
	 */
	void Unload();

	/**
	* Delete Task and Destroy this
	*/
	void Delete();

	/*
	 * Get the depth of this chunk
	 * @return Depth
	 */
	int GetDepth() const;

	/**
	 * Get the width of this chunk
	 * @return Width
	 */
	int Width() const;

	/**
	 * Get value in chunk space
	 * @param 	X	x coordinate in chunk space (between 0 and Width)
	 * @param 	Y	y coordinate in chunk space (between 0 and Width)
	 * @param 	Z	z coordinate in chunk space (between 0 and Width)
	 * @return	Value
	 */
	float GetValue(int X, int Y, int Z) const;

	/**
	 * Get color in chunk space
	 * @param 	X	x coordinate in chunk space (between 0 and Width)
	 * @param 	Y	y coordinate in chunk space (between 0 and Width)
	 * @param 	Z	z coordinate in chunk space (between 0 and Width)
	 * @return	Color
	 */
	FColor GetColor(int X, int Y, int Z) const;

	// ChunkHasHigherRes[TransitionDirection]
	TArray<bool, TFixedAllocator<6>> ChunkHasHigherRes;

protected:
	void Tick(float DeltaTime) override;

#if WITH_EDITOR
	bool ShouldTickIfViewportsOnly() const override;
#endif

private:
	UPROPERTY(EditAnywhere)
		UProceduralMeshComponent* PrimaryMesh;

	UPROPERTY(EditAnywhere)
		ULineBatchComponent* DebugLineBatch;

	// Toggle to manually init an update at the next tick
	UPROPERTY(EditAnywhere)
		bool bUpdate;

	// Minimal corner
	UPROPERTY(VisibleAnywhere)
		FIntVector Position;

	// Width = 16 * 2^Depth
	UPROPERTY(VisibleAnywhere)
		int Depth;

	AVoxelWorld* World;

	// Need to use ThreadedTask Section for PrimaryMesh
	bool bNeedSectionUpdate;

	// Async process task
	VoxelThread* Task;

	// If destruction of this chunk has been scheduled
	UPROPERTY(VisibleAnywhere)
		bool bNeedDeletion;
	// Time until destruction 
	UPROPERTY(VisibleAnywhere)
		float TimeUntilDeletion;

	// If adjacent chunks need update (when creating / destroying this)
	// Needed because it must be done at next tick, as ChunkOctree is partially initialized when Init is called
	bool bAdjacentChunksNeedUpdate;

	/**
	 * Get the adjacent chunk in direction
	 * @param	Direction	Direction of the chunk
	 * @return	Pointer to chunk if found, nullptr else
	 */
	AVoxelChunk* GetChunk(TransitionDirection Direction) const;

	/**
	 * Copy Task section to PrimaryMesh section
	 */
	void UpdateSection();
};