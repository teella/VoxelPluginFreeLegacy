#include "GothGirlSplineRoad.h"
#include "VoxelMinimal.h"
#include "VoxelWorld.h"
#include "VoxelIntBoxLibrary.h"
#include "VoxelPlaceableItems/VoxelPlaceableItemManager.h"
#include "VoxelGenerators/VoxelGeneratorCache.h"
#include "Examples/VDI_Capsule_Graph.h"
#include "Examples/VDI_Sphere_Graph.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"

namespace GothGirlSplineRoad
{
	int32 GetClosestSplinePointIndex(USplineComponent* Spline, float DistanceAlongSpline)
	{
		FVector LocationAtDistance = Spline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		int32 NumPoints = Spline->GetNumberOfSplinePoints();

		int32 ClosestPointIndex = INDEX_NONE;
		float ClosestDistanceSquared = FLT_MAX;

		for (int32 i = 0; i < NumPoints; ++i)
		{
			FVector PointLocation = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			float DistanceSquared = FVector::DistSquared(PointLocation, LocationAtDistance);

			if (DistanceSquared < ClosestDistanceSquared)
			{
				ClosestDistanceSquared = DistanceSquared;
				ClosestPointIndex = i;
			}
		}

		return ClosestPointIndex;
	}

	const FInterpCurvePointQuat DummyPointRotation(0.0f, FQuat::Identity);

	const FInterpCurvePointQuat& GetRotationPointSafe(USplineComponent* Spline, int32 PointIndex)
	{
		const TArray<FInterpCurvePointQuat>& Points = Spline->SplineCurves.Rotation.Points;
		const int32 NumPoints = Points.Num();
		if (NumPoints > 0)
		{
			const int32 ClampedIndex = (Spline->IsClosedLoop() && PointIndex >= NumPoints) ? 0 : FMath::Clamp(PointIndex, 0, NumPoints - 1);
			return Points[ClampedIndex];
		}
		else
		{
			return DummyPointRotation;
		}
	}
};

AGothGirlSplineRoad::AGothGirlSplineRoad()
	:Super()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Spline = CreateDefaultSubobject<USplineComponent>(FName(TEXT("SplineComp")));
	SetRootComponent(Spline);
	
	Mesh = LoadObject<UStaticMesh>(this, TEXT("/Game/Brushify/Meshes/Roads/Meshes/S_RoadPlane"));
	Materials.Add(LoadObject<UMaterialInterface>(this, TEXT("/Game/Brushify/Meshes/Roads/Materials/Pebbles/MI_Road_Pebbles_03_Inst")));

#if WITH_EDITOR
	static bool bAddedDelegate = false;
	if (!bAddedDelegate)
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.AddLambda([](UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
			{
				if (!PropertyChangedEvent.Property)
				{
					return;
				}
				auto* Component = Cast<USplineComponent>(Object);
				if (!Component)
				{
					return;
				}
				auto* DataItemActor = Cast<AGothGirlSplineRoad>(Component->GetOwner());
				if (!DataItemActor)
				{
					return;
				}
				DataItemActor->RefreshEditor(PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive);
			});
	}
#endif

}

#if WITH_EDITOR
void AGothGirlSplineRoad::RefreshEditor(bool ForceRefresh)
{
	if (ForceRefresh || LastEditorTime < FPlatformTime::Seconds())
	{
		//LOG_VOXEL(Log, TEXT("AGothGirlSplineRoad::RefreshEditor %f"), LastEditorTime);
		LastEditorTime = FPlatformTime::Seconds() + 0.3f;
		MakeSplineMesh();
	}
}
#endif

void AGothGirlSplineRoad::CreateFromCache()
{
	int Max = SplinePoints.Num();
	for (int i = 0; i < Max; i++)
	{
		USplineMeshComponent* SMC = nullptr;

		if (SplinePoints.IsValidIndex(i) && IsValid(SplinePoints[i].MeshComponent))
			SMC = SplinePoints[i].MeshComponent;
		else
			SMC = Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(), true, FTransform::Identity, false));

		if (IsValid(SMC))
		{
			SMC->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			SMC->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

			SMC->SetMobility(EComponentMobility::Movable);
			SMC->AttachToComponent(Spline, FAttachmentTransformRules::SnapToTargetIncludingScale);
			SMC->SetRelativeTransform(FTransform::Identity);

			SMC->SetStaticMesh(Mesh);
			for (int m = 0; m < Materials.Num(); m++)
			{
				SMC->SetMaterial(m, Materials[m]);
			}

			SMC->SetStartAndEnd(SplinePoints[i].Start, SplinePoints[i].StartTangent, SplinePoints[i].End, SplinePoints[i].EndTangent, true);
			SMC->SetStartRollDegrees(SplinePoints[i].StartRotation.Roll);
			SMC->SetEndRollDegrees(SplinePoints[i].EndRotation.Roll);
			SplinePoints[i].MeshComponent = SMC;
		}
	}
}

void AGothGirlSplineRoad::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		TArray<USceneComponent*> Comps;
		Spline->GetChildrenComponents(true, Comps);

		if (Comps.Num() == 0 && SplinePoints.Num() > 0)
		{
			CreateFromCache();
		}
	}
}

#if WITH_EDITOR
void AGothGirlSplineRoad::TransformUpdated(USceneComponent* InRootComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	RefreshEditor(true);
}
#endif

void AGothGirlSplineRoad::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	if (!UpdateHandle.IsValid())
		UpdateHandle = GetRootComponent()->TransformUpdated.AddUObject(this, &AGothGirlSplineRoad::TransformUpdated);
#endif

	if (SplinePoints.Num() > 0)
	{
		CreateFromCache();
	}
	else
	{
		MakeSplineMesh();
	}

	//LOG_VOXEL(Log, TEXT("AGothGirlSplineRoad::OnConstruction"));
}

void AGothGirlSplineRoad::MakeSplineMesh()
{
	if (IsValid(Mesh))
	{
		if (LastPointCount != Spline->GetNumberOfSplinePoints())
		{
			if (LastPointCount > Spline->GetNumberOfSplinePoints())
			{
				for (FGothGirlRoadInfo& Info : SplinePoints)
				{
					if (IsValid(Info.MeshComponent))
					{
						Info.MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
						Info.MeshComponent->DestroyComponent();
					}
					Info.MeshComponent = nullptr;
				}

				SplinePoints.Reset();
			}

			LastPointCount = Spline->GetNumberOfSplinePoints();
			//zero out latest spline point rotation
			if (SnappingOn)
				Spline->SetRotationAtSplinePoint(LastPointCount - 1, FRotator::ZeroRotator, ESplineCoordinateSpace::Local);
		}

		FTransform CurrentParentTransform = GetActorTransform();
		bool TransformEqual = UKismetMathLibrary::NearlyEqual_TransformTransform(LastParentTransform, CurrentParentTransform);

		FBoxSphereBounds Bounds = Mesh->GetBounds();
		float MeshLength{ 200.0f };
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		MeshLength = GetActorScale3D().X * Bounds.BoxExtent.X;

		if (MeshLength <= 0.0f)
			MeshLength = 200.0f;

		int Max = FMath::RoundToInt((float)Spline->GetSplineLength() / MeshLength) - 1;

		//LOG_VOXEL(Log, TEXT("Max %i||%i"), Max, SplinePoints.Num());

		for (int i = 0; i < Max; i++)
		{
			USplineMeshComponent* SMC = nullptr;

			if (SplinePoints.IsValidIndex(i) && IsValid(SplinePoints[i].MeshComponent))
				SMC = SplinePoints[i].MeshComponent;
			else
				SMC = Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(), true, FTransform::Identity, false));

			if (IsValid(SMC))
			{
				SMC->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				SMC->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

				SMC->SetMobility(EComponentMobility::Movable);
				SMC->AttachToComponent(Spline, FAttachmentTransformRules::SnapToTargetIncludingScale);
				SMC->SetRelativeTransform(FTransform::Identity);

				SMC->SetStaticMesh(Mesh);
				for (int m = 0; m < Materials.Num(); m++)
				{
					SMC->SetMaterial(m, Materials[m]);
				}

				float StartDist = MeshLength * (float)i;
				float EndDist = MeshLength * (float)(i + 1);
				FVector Start = Spline->GetLocationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
				FVector End = Spline->GetLocationAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::World);
				FHitResult Hit;
				
				FVector UpVector = Spline->GetUpVectorAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);

				FVector TraceStart = Start + (UpVector * Bounds.BoxExtent.Z);
				FVector TraceEnd = Start + (UpVector * -TraceDistance);

				FRotator StartRot = Spline->GetRotationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::Local);
				FRotator EndRot = Spline->GetRotationAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::Local);
				FVector StartTangent = UKismetMathLibrary::ClampVectorSize(Spline->GetTangentAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::Local), 0.0f, MeshLength);
				FVector EndTangent = UKismetMathLibrary::ClampVectorSize(Spline->GetTangentAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::Local), 0.0f, MeshLength);

				FVector RawStart = Spline->GetLocationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::Local);
				FVector RawEnd = Spline->GetLocationAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::Local);
				FRotator RawStartRot = StartRot;
				FRotator RawEndRot = EndRot;
				FVector RawStartTangent = StartTangent;
				FVector RawEndTangent = EndTangent;

				if (TransformEqual && SplinePoints.IsValidIndex(i))
				{
					if (SplinePoints[i].RawStart == RawStart &&
						SplinePoints[i].RawStartRotation == RawStartRot &&
						SplinePoints[i].RawStartTangent == RawStartTangent &&

						SplinePoints[i].RawEnd == RawEnd &&
						SplinePoints[i].RawEndRotation == RawEndRot &&
						SplinePoints[i].RawEndTangent == RawEndTangent)
					{
						continue;
					}
				}

				if (SnappingOn && DoLineTraceSingle(Hit, TraceStart, TraceEnd, ActorsToIgnore))
				{
					if (Hit.bBlockingHit)
					{
						Start = Spline->GetComponentTransform().InverseTransformPosition(Hit.ImpactPoint) + ((Bounds.BoxExtent * GetActorScale3D() * ZOffset).Z * UpVector);
						StartRot = FRotationMatrix::MakeFromXZ(Spline->GetTangentAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World), Hit.Normal).Rotator();
						StartRot = Spline->GetComponentTransform().InverseTransformRotation(StartRot.Quaternion()).Rotator();
						// Recalculate the tangent in the world space and then convert to local space
						FVector TangentWorld = Spline->GetTangentAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
						FVector TangentLocal = Spline->GetComponentTransform().InverseTransformVector(TangentWorld);
						StartTangent = UKismetMathLibrary::ClampVectorSize(TangentLocal, 0.0f, MeshLength);
					}
				}
				else
				{
					Start = RawStart;
				}

				UpVector = Spline->GetUpVectorAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::World);
				TraceStart = End + (UpVector * Bounds.BoxExtent.Z);
				TraceEnd = End + (UpVector * -TraceDistance);
				if (SnappingOn && DoLineTraceSingle(Hit, TraceStart, TraceEnd, ActorsToIgnore))
				{
					if (Hit.bBlockingHit)
					{
						End = Spline->GetComponentTransform().InverseTransformPosition(Hit.ImpactPoint) + ((Bounds.BoxExtent * GetActorScale3D() * ZOffset).Z * UpVector);
						EndRot = FRotationMatrix::MakeFromXZ(Spline->GetTangentAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::World), Hit.Normal).Rotator();
						EndRot = Spline->GetComponentTransform().InverseTransformRotation(EndRot.Quaternion()).Rotator();
						// Recalculate the tangent in the world space and then convert to local space
						FVector TangentWorld = Spline->GetTangentAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::World);
						FVector TangentLocal = Spline->GetComponentTransform().InverseTransformVector(TangentWorld);
						EndTangent = UKismetMathLibrary::ClampVectorSize(TangentLocal, 0.0f, MeshLength);
					}
				}
				else
				{
					End = RawEnd;
				}


				int Index = GothGirlSplineRoad::GetClosestSplinePointIndex(Spline, StartDist);
				FInterpCurvePointQuat rtn = GothGirlSplineRoad::GetRotationPointSafe(Spline, Index);

				//LOG_VOXEL(Log, TEXT("GetClosestSplinePointIndex %i [%s](%f)"), Index, *rtn.OutVal.Rotator().ToString(), rtn.InVal);
				if (rtn.OutVal != FQuat::Identity)
					StartRot = (StartRot.Quaternion() * Spline->GetRotationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::Local).Quaternion()).Rotator();

				Index = GothGirlSplineRoad::GetClosestSplinePointIndex(Spline, EndDist);
				rtn = GothGirlSplineRoad::GetRotationPointSafe(Spline, Index);
				if (rtn.OutVal != FQuat::Identity)
					EndRot = (EndRot.Quaternion() * Spline->GetRotationAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::Local).Quaternion()).Rotator();

				SMC->SetStartAndEnd(Start, StartTangent, End, EndTangent, true);
				SMC->SetStartRollDegrees(StartRot.Roll);
				SMC->SetEndRollDegrees(EndRot.Roll);

				if (SplinePoints.IsValidIndex(i))
				{
					SplinePoints[i].Start = Start;
					SplinePoints[i].StartTangent = StartTangent;
					SplinePoints[i].StartRotation = StartRot;

					SplinePoints[i].End = End;
					SplinePoints[i].EndTangent = EndTangent;
					SplinePoints[i].EndRotation = EndRot;

					SplinePoints[i].RawStart = RawStart;
					SplinePoints[i].RawStartTangent = RawStartTangent;
					SplinePoints[i].RawStartRotation = RawStartRot;

					SplinePoints[i].RawEnd = RawEnd;
					SplinePoints[i].RawEndTangent = RawEndTangent;
					SplinePoints[i].RawEndRotation = RawEndRot;
				}
				else
					SplinePoints.Add({ Start, StartTangent, StartRot, End, EndTangent, EndRot, RawStart, RawStartTangent, RawStartRot, RawEnd, RawEndTangent, RawEndRot, SMC });
			}
		}

		if (Max > 0 && SplinePoints.Num() > 0 && Max < SplinePoints.Num())
		{
			for (int i = SplinePoints.Num() - 1; i > Max - 1; i--)
			{
				if (IsValid(SplinePoints[i].MeshComponent))
				{
					SplinePoints[i].MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
					SplinePoints[i].MeshComponent->DestroyComponent();
				}
				SplinePoints.RemoveAt(i);
			}
		}

		LastParentTransform = CurrentParentTransform;
	}
}

bool AGothGirlSplineRoad::DoLineTraceSingle(FHitResult& OutHit, const FVector& Start, const FVector& End, const TArray<AActor*> ActorsToIgnore, FLinearColor TraceColour)
{
	return UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		Start,
		End,
		TraceQueryType,
		false,
		ActorsToIgnore,
		DrawDebugType,
		OutHit,
		true,
		TraceColour
	);
}
