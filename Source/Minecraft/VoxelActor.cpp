// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelActor.h"
#include "Minecraft.h"
//#include "F:/Access/Documents/Unreal Projects/Minecraft/Plugins/SimplexNoise-master/Source/SimplexNoise/Private/"

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
	int32 chunkLineElementsExt = chunkLineElements + 2;
	int32 chunkLineElementsExtP2 = chunkLineElements * chunkLineElements;
	int32 chunkTotalElementsExt = chunkLineElements * chunkLineElements * chunkZElements;

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
	FRandomStream RandomStream = FRandomStream(randomSeed);
	TArray<FIntVector> treeCenters;

	chunkFields.SetNumUninitialized(chunkTotalElements);

	TArray<int32> noise = calculateNoise();

	for (int32 x = 0; x < chunkLineElementsExt; x++)
	{
		for (int32 y = 0; y < chunkLineElementsExt; y++)
		{
			for (int32 z = 0; z < chunkZElements; z++)
			{
				int32 index = x + (y * chunkLineElementsExt) + (z * chunkLineElementsP2Ext);

				if (z == 30 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = 1;
				else if (28 + noise[x + y * chunkLineElementsExt] <= z  && z < 30 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = 2;
				else if (z < 28 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = 3;
				else chunkFields[index] = 0;
			}
		}
	}

	//smaller for trees to avoid shared blocks
	for (int32 x = 2; x < chunkLineElementsExt; x++)
	{
		for (int32 y = 2; y < chunkLineElementsExt; y++)
		{
			for (int32 z = 0; z < chunkZElements; z++)
			{
				if (RandomStream.FRand() < 0.01 && z == 31 + noise[x + y * chunkLineElementsExt]) treeCenters.Add(FIntVector(x, y, z));
			}
		}
	}

	//for (int32 Index = 0; Index != treeCenters.Num(); ++Index)
	//{
	//	FIntVector treeCenter = treeCenters[Index];
	for (FIntVector treeCenter : treeCenters)
	{
		int32 tree_height = RandomStream.RandRange(4, 6);
		int32 randomX = RandomStream.RandRange(0, 2);
		int32 randomY = RandomStream.RandRange(0, 2);
		int32 randomZ = RandomStream.RandRange(0, 2);


		//leaves
		for (int32 tree_x = -2; tree_x < 3; tree_x++)
		{
			for (int32 tree_y = -2; tree_y < 3; tree_y++)
			{
				for (int32 tree_z = -2; tree_z < 3; tree_z++)
				{
					if (inRange(tree_x + treeCenter.X, chunkLineElementsExt) && inRange(tree_y + treeCenter.Y,chunkLineElementsExt) && inRange(tree_z + treeCenter.Z, chunkZElements))
					{
						float radius = FVector(tree_x * randomX, tree_y * randomY, tree_z * randomZ).Size();

						if (radius <= 2.8)
						{
							if (RandomStream.FRand() < 0.5 || radius < 1.2)
<<<<<<< HEAD
								chunkFields[treeCenter.X + tree_x + ((treeCenter.Y + tree_y) * chunkLineElementsExt) + ((treeCenter.Z + tree_z + tree_height) * chunkLineElementsP2Ext)] = 21;
=======
							//	chunkFields[treeCenter.X + tree_x + (chunkLineElements * (treeCenter.Y + tree_y)) + (chunkLineElementsP2 * (treeCenter.Z + tree_z + tree_height))] = 3;
								chunkFields[treeCenter.X + tree_x + ((treeCenter.Y + tree_y) * chunkLineElements) + ((treeCenter.Z + tree_z + tree_height) * chunkLineElementsP2)] = 3;
>>>>>>> parent of d1d2111... Textured Log and Windy Leaves
						}
					}
				}
			}
		}
<<<<<<< HEAD
		//tree trunk
		for (int32 h = 0; h < tree_height; h++)
		{
			chunkFields[treeCenter.X + (treeCenter.Y * chunkLineElementsExt) + ((treeCenter.Z + h) * chunkLineElementsP2Ext)] = 22;
		}
=======
>>>>>>> parent of d1d2111... Textured Log and Windy Leaves
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
				int32 index = (x + 1) + ((y + 1) * chunkLineElementsExt) + (z * chunkLineElementsP2Ext);
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

					//add faces, verticies, uvs, and normals
					int triangle_num = 0;
					for (int i = 0; i < 6; i++)
					{
						int newIndex = index + bMask[i].X + (bMask[i].Y * chunkLineElementsExt) + (bMask[i].Z * chunkLineElementsP2Ext);

						bool flag = false;
						if (meshIndex >= 20) flag = true;
						//else if ((x + bMask[i].X < chunkLineElementsExt) && (x + bMask[i].X >= 0) && (y + bMask[i].Y < chunkLineElementsExt) && (y + bMask[i].Y >= 0))
						//{
						else if (newIndex < chunkFields.Num() && newIndex >= 0)
							if (chunkFields[newIndex] < 1) flag = true;
						//}
						//else flag = true;

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
	aa.SetNum(chunkLineElementsP2Ext);
	return aa;
}

void AVoxelActor::setVoxel(FVector localPos, int32 value)
{
	int32 x = localPos.X / voxelSize;
	int32 y = localPos.Y / voxelSize;
	int32 z = localPos.Z / voxelSize;

	int32 index = x + (y * chunkLineElementsExt) + (z * chunkLineElementsP2Ext);

	chunkFields[index] = value;

	UpdateMesh();
}

int32 AVoxelActor::getVoxel(FVector localPos)
{
	int32 x = localPos.X / voxelSize;
	int32 y = localPos.Y / voxelSize;
	int32 z = localPos.Z / voxelSize;

	int32 index = x + (y * chunkLineElementsExt) + (z * chunkLineElementsP2Ext);

	return chunkFields[index];
	UpdateMesh();
}

bool AVoxelActor::inRange(int32 value, int32 range)
{
	return (value >= 0 && value < range);
}

//TArray<int32> AVoxelActor::calculateNoise()
//{
//	TArray<int32> noises;
//	noises.Reserve(chunkLineElementsExt * chunkLineElementsExt);
//	for (int32 y = -1; y <= chunkLineElements; y++)
//	{
//		for (int32 x = -1; x <= chunkLineElements; x++)
//		{
//			float noiseValue =
//				USimplexNoiseBPLibrary::SimplexNoise2D((chunkXindex * chunkLineElements + x) * 0.01f, (chunkYIndex * chunkLineElements + y) * 0.01f) * 4 +
//				USimplexNoiseBPLibrary::SimplexNoise2D((chunkXindex * chunkLineElements + x) * 0.01f, (chunkYIndex * chunkLineElements + y) * 0.01f) * 8 +
//				USimplexNoiseBPLibrary::SimplexNoise2D((chunkXindex * chunkLineElements + x) * 0.004f, (chunkYIndex * chunkLineElements + y) * 0.004f) * 16 +
//				FMath::Clamp(USimplexNoiseBPLibrary::SimplexNoise2D((chunkXindex * chunkLineElements + x) * 0.05f, (chunkYIndex * chunkLineElements + y)  * 0.05f), 0.0f, 5.0f) * 4;
//				noises.Add(FMath::FloorToInt(noiseValue));
//		}
//	}
//	return noises;
//}