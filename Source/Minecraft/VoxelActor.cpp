// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelActor.h"
#include "Minecraft.h"


const int32 bTriangles[] = { 2, 1, 0, 0, 3, 2 };
const FVector2D bUVs[] = { FVector2D(0.0, 0.0), FVector2D(0.0, 1.0), FVector2D(1.0, 1.0) , FVector2D(1.0, 0.0) };
const FVector bNormals0[] = { FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1) };
const FVector bNormals1[] = { FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1) };
const FVector bNormals2[] = { FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0) };
const FVector bNormals3[] = { FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0) };
const FVector bNormals4[] = { FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0) };
const FVector bNormals5[] = { FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0) };
const FVector bMask[] = { FVector(0.0, 0.0, 1.0), FVector(0.0, 0.0, -1.0), FVector(0.0, 1.0, 0.0), FVector(0.0, -1.0, 0.0), FVector(1.0, 0.0, 0.0), FVector(-1.0, 0.0, 0.0) };

// Sets default values
AVoxelActor::AVoxelActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVoxelActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVoxelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVoxelActor::OnConstruction(const FTransform& Transform)
{
	chunkZElements = 80;
	chunkTotalElements = chunkLineElements * chunkLineElements * chunkZElements;
	chunkLineElementsP2 = chunkLineElements * chunkLineElements;
	voxelSizeHalf = voxelSize / 2;

	FString string = "Voxel_" + FString::FromInt(chunkXindex) + "_" + FString::FromInt(chunkYIndex);
	FName name = FName(*string);
	proceduralComponent = NewObject<UProceduralMeshComponent>(this, name);
	proceduralComponent->RegisterComponent();

	RootComponent = proceduralComponent;
	RootComponent->SetWorldTransform(Transform);

	Super::OnConstruction(Transform);

	GenerateChunk();
	UpdateMesh();
}

void AVoxelActor::GenerateChunk()
{
	chunkFields.SetNumUninitialized(chunkTotalElements);

	TArray<int32> noise = calculateNoise();

	for (int32 x = 0; x < chunkLineElements; x++)
	{
		for (int32 y = 0; y < chunkLineElements; y++)
		{
			for (int32 z = 0; z < chunkZElements; z++)
			{
				int32 index = x + (y * chunkLineElements) + (z * chunkLineElementsP2);

				if (z == 30 + noise[x + y * chunkLineElements]) chunkFields[index] = 1;
				else if (28 + noise[x + y * chunkLineElements] <= z  && z < 30 + noise[x + y * chunkLineElements]) chunkFields[index] = 2;
				else if (z < 28 + noise[x + y * chunkLineElements]) chunkFields[index] = 3;
				else chunkFields[index] = 0;
			}
		}
	}
}

void AVoxelActor::UpdateMesh()
{
	TArray<FMeshSection> meshSections;
	meshSections.SetNum(Materials.Num());
	int32 el_num = 0;

	for (int32 x = 0; x < chunkLineElements; x++)
	{
		for (int32 y = 0; y < chunkLineElements; y++)
		{
			for (int32 z = 0; z < chunkZElements; z++)
			{
				int32 index = x + (y * chunkLineElements) + (z * chunkLineElementsP2);
				int32 meshIndex = chunkFields[index];

				if (meshIndex > 0)
				{
					meshIndex--;

					TArray<FVector> &Vertices = meshSections[meshIndex].Vertices;
					TArray<int32> &Triangles = meshSections[meshIndex].Triangles;
					TArray<FVector> &Normals = meshSections[meshIndex].Normals;
					TArray<FVector2D> &UVs = meshSections[meshIndex].UVs;
					TArray<FProcMeshTangent> &Tangent = meshSections[meshIndex].Tangents;
					TArray<FColor> &VertexColors = meshSections[meshIndex].VertexColors;

					int32 elementID = meshSections[meshIndex].elementID;

					int triangle_num = 0;
					for (int i = 0; i < 6; i++)
					{
						int newIndex = index + bMask[i].X + (bMask[i].Y * chunkLineElements) + (bMask[i].Z * chunkLineElementsP2);

						bool flag = false;
						if (meshIndex >= 20) flag = true;
						else if ((x + bMask[i].X < chunkLineElements) && (x + bMask[i].X >= 0) && (y + bMask[i].Y < chunkLineElements) && (y + bMask[i].Y >= 0))
						{
							if (newIndex < chunkFields.Num() && newIndex >= 0)
								if (chunkFields[newIndex] < 1) flag = true;
						}
						else flag = true;

						if (flag)	
						{
							Triangles.Add(bTriangles[0] + triangle_num + elementID);
							Triangles.Add(bTriangles[1] + triangle_num + elementID);
							Triangles.Add(bTriangles[2] + triangle_num + elementID);
							Triangles.Add(bTriangles[3] + triangle_num + elementID);
							Triangles.Add(bTriangles[4] + triangle_num + elementID);
							Triangles.Add(bTriangles[5] + triangle_num + elementID);
							triangle_num += 4; //add 4 verticies to next triangle

							switch (i)
							{
							case 0: {
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals0, ARRAY_COUNT(bNormals0));
								break;
							}
							case 1: {
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals1, ARRAY_COUNT(bNormals1));
								break;
							}
							case 2: {
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals2, ARRAY_COUNT(bNormals2));
								break;
							}
							case 3: {
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals3, ARRAY_COUNT(bNormals3));
								break;
							}
							case 4: {
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals4, ARRAY_COUNT(bNormals4));
								break;
							}
							case 5: {
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals5, ARRAY_COUNT(bNormals5));
								break;
							}
							}

							UVs.Append(bUVs, ARRAY_COUNT(bUVs));

							FColor color = FColor(255, 255, 255, i);
							VertexColors.Add(color); VertexColors.Add(color); VertexColors.Add(color); VertexColors.Add(color);
						}
					}
					el_num += triangle_num;
					meshSections[meshIndex].elementID += triangle_num;
				}
			}
		}
	}
	proceduralComponent->ClearAllMeshSections();
	for (int i = 0; i < meshSections.Num(); i++)
	{
		if (meshSections[i].Vertices.Num() > 0)
		{
			proceduralComponent->CreateMeshSection(i, meshSections[i].Vertices, meshSections[i].Triangles, meshSections[i].Normals, meshSections[i].UVs, meshSections[i].VertexColors, meshSections[i].Tangents, true);
		}
	}

	int s = 0;
	while (s < Materials.Num())
	{
		proceduralComponent->SetMaterial(s, Materials[s]);
		s++;
	}
}

TArray<int32> AVoxelActor::calculateNoise_Implementation()
{
	TArray<int32> aa;
	aa.SetNum(chunkLineElementsP2);
	return aa;
}

void AVoxelActor::setVoxel(FVector localPos, int32 value)
{
	int32 x = localPos.X / voxelSize;
	int32 y = localPos.Y / voxelSize;
	int32 z = localPos.Z / voxelSize;

	int32 index = x + (y * chunkLineElements) + (z * chunkLineElementsP2);

	chunkFields[index] = value;

	UpdateMesh();
}
