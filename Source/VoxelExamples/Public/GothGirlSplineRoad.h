#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelPlaceableItems/Actors/VoxelDataItemActor.h"
#include "Components/SplineComponent.h"
#include "VoxelGenerators/VoxelGeneratorCache.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GothGirlSplineRoad.generated.h"

class AVoxelWorld;
class UMaterialInterface;
class UStaticMesh;
class USplineMeshComponent;

USTRUCT(BlueprintType, Blueprintable)
struct FGothGirlRoadInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|SplineRoad")
	FVector Start;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|SplineRoad")
	FVector StartTangent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|SplineRoad")
	FRotator StartRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|SplineRoad")
	FVector End;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|SplineRoad")
	FVector EndTangent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|SplineRoad")
	FRotator EndRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|SplineRoad")
	FVector RawStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|SplineRoad")
	FVector RawStartTangent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|SplineRoad")
	FRotator RawStartRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|SplineRoad")
	FVector RawEnd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|SplineRoad")
	FVector RawEndTangent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|SplineRoad")
	FRotator RawEndRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|SplineRoad")
	TObjectPtr<USplineMeshComponent> MeshComponent = nullptr;

	FGothGirlRoadInfo()
	{
		Start = FVector::ZeroVector;
		StartTangent = FVector::ZeroVector;
		StartRotation = FRotator::ZeroRotator;

		End = FVector::ZeroVector;
		EndTangent = FVector::ZeroVector;
		EndRotation = FRotator::ZeroRotator;

		RawStart = FVector::ZeroVector;
		RawStartTangent = FVector::ZeroVector;
		RawStartRotation = FRotator::ZeroRotator;

		RawEnd = FVector::ZeroVector;
		RawEndTangent = FVector::ZeroVector;
		RawEndRotation = FRotator::ZeroRotator;

		MeshComponent = nullptr;
	}

	FGothGirlRoadInfo(FVector InStart, FVector InStartTangent, FRotator InStartRotation,
					  FVector InEnd, FVector InEndTangent, FRotator InEndRotation,
					  FVector InRawStart, FVector InRawStartTangent, FRotator InRawStartRotation,
					  FVector InRawEnd, FVector InRawEndTangent, FRotator InRawEndRotation,
					  USplineMeshComponent * InMeshComponent)
	{
		Start = InStart;
		StartTangent = InStartTangent;
		StartRotation = InStartRotation;

		End = InEnd;
		EndTangent = InEndTangent;
		EndRotation = InEndRotation;

		RawStart = InRawStart;
		RawStartTangent = InRawStartTangent;
		RawStartRotation = InRawStartRotation;

		RawEnd = InRawEnd;
		RawEndTangent = InRawEndTangent;
		RawEndRotation = InRawEndRotation;

		MeshComponent = InMeshComponent;
	}
};


UCLASS(ClassGroup = (Voxel), BlueprintType, Blueprintable)
class VOXELEXAMPLES_API AGothGirlSplineRoad : public AActor
{
	GENERATED_BODY()

public:
	AGothGirlSplineRoad();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "GothGirl")
	void MakeSplineMesh();

#if WITH_EDITOR
	void RefreshEditor(bool ForceRefresh);
	double LastEditorTime{ 0.0f };
	void TransformUpdated(USceneComponent* InRootComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);
	FDelegateHandle UpdateHandle;
#endif

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GothGirl|Config", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> Spline = nullptr;

	UPROPERTY(Category = "GothGirl|Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool SnappingOn{ true };

	UPROPERTY(Category = "GothGirl|Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ZOffset{ 0.25f };

	UPROPERTY(Category = "GothGirl|Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float TraceDistance{ 200.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|Config", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMesh> Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GothGirl|Config", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UMaterialInterface>> Materials;

	UPROPERTY(Category = "GothGirl|Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ETraceTypeQuery> TraceQueryType{ ETraceTypeQuery::TraceTypeQuery1 };

	UPROPERTY(Category = "GothGirl|Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType{ EDrawDebugTrace::Type::None };

	bool DoLineTraceSingle(FHitResult& OutHit, const FVector& Start, const FVector& End, const TArray<AActor*> ActorsToIgnore, FLinearColor TraceColour = FLinearColor::Red);

	UPROPERTY()
	TArray<FGothGirlRoadInfo> SplinePoints;

	void CreateFromCache();

	int LastPointCount{ 0 };
	FTransform LastParentTransform{ FTransform::Identity };
};