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
		if (USplineMeshComponent* SMC = Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(), true, FTransform::Identity, false)))
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

void AGothGirlSplineRoad::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

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
		TArray<USceneComponent*> Comps;
		Spline->GetChildrenComponents(true, Comps);
		for (USceneComponent* comp : Comps)
		{
			if (USplineMeshComponent * SMC = Cast<USplineMeshComponent>(comp))
			{
				comp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
				comp->DestroyComponent();
			}
		}

		SplinePoints.Reset();

		FBoxSphereBounds Bounds = Mesh->GetBounds();
		float MeshLength{ 200.0f };
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		MeshLength = GetActorScale3D().X * Bounds.BoxExtent.X;

		if (MeshLength <= 0.0f)
			MeshLength = 200.0f;

		int Max = FMath::RoundToInt((float)Spline->GetSplineLength() / MeshLength);

		for (int i = 0; i < Max; i++)
		{
			if (USplineMeshComponent* SMC = Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(), true, FTransform::Identity, false)))
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
				FVector TraceStart = Start + (Spline->GetUpVector() * TraceDistance);
				FVector TraceEnd = Start + (Spline->GetUpVector() * -TraceDistance);

				FRotator StartRot = Spline->GetRotationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
				FRotator EndRot = Spline->GetRotationAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::World);
				FVector StartTangent = UKismetMathLibrary::ClampVectorSize(Spline->GetTangentAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::Local), 0.0f, MeshLength);
				FVector EndTangent = UKismetMathLibrary::ClampVectorSize(Spline->GetTangentAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::Local), 0.0f, MeshLength);

				if (DoLineTraceSingle(Hit, TraceStart, TraceEnd, ActorsToIgnore))
				{
					if (Hit.bBlockingHit)
					{
						Start = (Hit.ImpactPoint - Spline->GetComponentLocation()) + ((Bounds.BoxExtent * GetActorScale3D() * ZOffset).Z * Spline->GetUpVector());
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
					Start = Spline->GetLocationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::Local);
				}

				TraceStart = End + (Spline->GetUpVector() * TraceDistance);
				TraceEnd = End + (Spline->GetUpVector() * -TraceDistance);
				if (DoLineTraceSingle(Hit, TraceStart, TraceEnd, ActorsToIgnore))
				{
					if (Hit.bBlockingHit)
					{
						End = (Hit.ImpactPoint - Spline->GetComponentLocation()) + ((Bounds.BoxExtent * GetActorScale3D() * ZOffset).Z * Spline->GetUpVector());
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
					End = Spline->GetLocationAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::Local);
				}

				if (Spline->GetRotationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::Local) != FRotator::ZeroRotator)
					StartRot = (StartRot.Quaternion() * Spline->GetRotationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::Local).Quaternion()).Rotator();
				if (Spline->GetRotationAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::Local) != FRotator::ZeroRotator)
					EndRot = (EndRot.Quaternion() * Spline->GetRotationAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::Local).Quaternion()).Rotator();

				SMC->SetStartAndEnd(Start, StartTangent, End, EndTangent, true);
				SMC->SetStartRollDegrees(StartRot.Roll);
				SMC->SetEndRollDegrees(EndRot.Roll);

				SplinePoints.Add({ Start, StartTangent, StartRot, End, EndTangent, EndRot });
			}
		}
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
