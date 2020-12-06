#pragma once

#include <d3d11.h>
#include <fbxsdk.h>
#include <Shlwapi.h>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "Misc.h"
#include "Vector.h"
#include "ConstantBuffer.h"
#include "Function.h"
#include "PlayBackController.h"
#undef max
#undef min

#include<cereal/cereal.hpp>
#include<cereal/archives/binary.hpp>
#include<cereal/types/memory.hpp>
#include<cereal/types/vector.hpp>

// FbxDouble4 Å® XMFLOAT4x4
inline FLOAT4X4 FbxAMatrixToFloat4x4(const FbxAMatrix& fbx_value)
{
	return FLOAT4X4(
		static_cast<float>(fbx_value[0][0]),
		static_cast<float>(fbx_value[0][1]),
		static_cast<float>(fbx_value[0][2]),
		static_cast<float>(fbx_value[0][3]),
		static_cast<float>(fbx_value[1][0]),
		static_cast<float>(fbx_value[1][1]),
		static_cast<float>(fbx_value[1][2]),
		static_cast<float>(fbx_value[1][3]),
		static_cast<float>(fbx_value[2][0]),
		static_cast<float>(fbx_value[2][1]),
		static_cast<float>(fbx_value[2][2]),
		static_cast<float>(fbx_value[2][3]),
		static_cast<float>(fbx_value[3][0]),
		static_cast<float>(fbx_value[3][1]),
		static_cast<float>(fbx_value[3][2]),
		static_cast<float>(fbx_value[3][3])
	);
}

#define MAX_BONE_INFLUENCES 4
#define MAX_BONES 128

namespace Source
{
	namespace ModelResource
	{
		struct Vertex
		{
			VECTOR3F position;
			VECTOR3F normal;
			VECTOR2F texcoord;
			float boneWeights[MAX_BONE_INFLUENCES] = { 1.0f,.0f,.0f,.0f };
			int boneIndices[MAX_BONE_INFLUENCES] = {};

			template<class T>
			void serialize(T& archive)
			{
				archive
				(
					position,
					normal,
					texcoord,
					boneWeights,
					boneIndices
				);
			}
		};

		struct HierarchyNode
		{
			FbxNodeAttribute::EType attribute;
			std::string name;
			std::string parentName;
			std::vector<HierarchyNode> chirdlen;

			template<class T>
			void serialize(T& archive)
			{
				archive(attribute, name, parentName,chirdlen);
			}
		};

		//í«â¡
		struct AnimationTake
		{
			std::string name;
			int numberOfKeyframes = 0;
			float samplingRate = 0;
			float period = 0.0f;
			float start = 0.0f;
			float end = 0.0f;
			float duration = 0.0f;
			float weight = 0.0f;

			Source::Blend::PlayBackController animtion;
			template<class T>
			void serialize(T& archive)
			{
				archive(name, samplingRate,
					period,start,end,duration);
			}
		};
		void FetchAnimationTakes(FbxScene* scene, std::vector<ModelResource::AnimationTake>& animationTakes, int samplingRate/*0:default value*/);

		struct Bone
		{
			std::string name;
			std::string parentName;
			FLOAT4X4 transform; // bone transform in global space
			FLOAT4X4 transformToParent; // from bone space to parent bone space

			template<class T>
			void serialize(T& archive)
			{
				archive(name, parentName, transform, transformToParent);
			}
		};

		struct Skeletal
		{
			std::vector<Bone> bones;

			template<class T>
			void serialize(T& archive)
			{
				archive(bones);
			}
		};

		struct ActorTransform
		{
			FLOAT4X4 transform;// model transform in global space
			template<class T>
			void serialize(T& archive)
			{
				archive(transform);
			}
		};

		struct Animation
		{
			std::string name;
			std::vector<Skeletal> skeletalTransform; // Bone transforms in global space
			std::vector<ActorTransform> actorTransforms; // Actor transforms in global space

			std::vector<FLOAT4X4> locals;
			std::vector<FLOAT4X4> localsToParent;
			template<class T>
			void serialize(T& archive)
			{
				archive(name, skeletalTransform, actorTransforms);
			}
		};
		void FetchAnimation(FbxMesh* fbxMesh, const AnimationTake& animationTake, Animation& animation);
		void FetchOffsetMatrices(FbxMesh* fbxMesh, std::vector<ModelResource::Bone>& offsetTransforms);

		struct Material
		{
			std::string name;
			unsigned int indexStart = 0;	// start number of index buffer
			unsigned int indexCount = 0;	// number of vertices (indices)

			struct property
			{
				float factor = 1;
				VECTOR4F color;
				std::string textureFilename;
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dummyShaderResourceView;

				template<class T>
				void serialize(T& archive)
				{
					archive
					(
						factor,
						color,
						textureFilename
					);
				}
			};

			property ambient;
			property diffuse;
			property specular;
			property bump;
			property normalMap;

			template<class T>
			void serialize(T& archive)
			{
				archive(name, indexStart, indexCount, ambient, diffuse, specular, bump, normalMap);
			}
		};

		struct BoundingBox
		{
			VECTOR3F min = { +FLT_MAX, +FLT_MAX, +FLT_MAX };
			VECTOR3F max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

			template<class T>
			void serialize(T& archive)
			{
				archive(min.x, min.y, min.z, max.x, max.y, max.z);
			}
		};

		struct Face
		{
			VECTOR3F position[3];
			int materialIndex = 0;
			template <class Archive>
			void serialize(Archive& archive)
			{
				archive(
					position[0],
					position[1],
					position[2],
					materialIndex);
			}
		};
		void FetchMaterials(FbxMesh* fbxMesh, std::vector<ModelResource::Material>& materials);
		void FetchVerticesAndIndices(FbxMesh* fbxMesh, std::vector<ModelResource::Vertex>& vertices,
			std::vector<unsigned int>& indices, std::vector<ModelResource::Material>& materials,
			std::vector<ModelResource::Face>& faces, FLOAT4X4& globalTransform);



		template <class T>
		struct Substance
		{
			struct Mesh
			{
				std::string name;
				std::vector<T> vertices;
				std::vector<u_int> indices;

				Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
				Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

				FLOAT4X4 globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
				std::vector<Bone> offsetTransforms; // initial transforms from model space to bone spaces
				std::vector<Material> materials;
				std::vector<Animation> animations;
				std::vector<Face> faces;
				BoundingBox boundingBox;


				const char* FindMaterial(int indexOfTriangle) const
				{
					unsigned int indexOfIndices = 3 * indexOfTriangle;
					for (const Material& material : materials)
					{
						if (material.indexStart <= indexOfIndices && material.indexStart + material.indexCount > indexOfIndices)
						{
							return material.name.empty() ? "" : material.name.c_str();
						}
					}
					return 0;
				}

				const Bone* FindOffsetBone(const char* boneName) const
				{
					for (const Bone& bone : offsetTransforms)
					{
						if (bone.name == boneName)
						{
							return &bone;
						}
					}
					return 0;
				}

				template<class T>
				void serialize(T& archive)
				{
					archive
					(
						name, vertices, indices, materials, animations, faces,
						globalTransform, offsetTransforms
						);
				}
			};


			Substance() = default;
			~Substance() = default;
			Substance(const char* fbxFilename, int animationSamplingRate = 0)
			{
				if (PathFileExistsA((std::string(fbxFilename) + ".bin").c_str()))
				{
					std::ifstream ifs;
					ifs.open((std::string(fbxFilename) + ".bin").c_str(), std::ios::binary);
					cereal::BinaryInputArchive i_archive(ifs);
					i_archive(*this);
				}
				else
				{
					CreateFromFbx(fbxFilename, animationSamplingRate);
				}
			}

		public:
			void CreateRenderObjects(ID3D11Device* device)
			{
				for (Mesh& mesh : _meshes)
				{
					HRESULT hr;
					{
						D3D11_BUFFER_DESC bufferDesc = {};
						D3D11_SUBRESOURCE_DATA subresourceData = {};

						bufferDesc.ByteWidth = static_cast<UINT>(sizeof(T) * mesh.vertices.size());
						bufferDesc.Usage = D3D11_USAGE_DEFAULT;
						bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
						bufferDesc.CPUAccessFlags = 0;
						bufferDesc.MiscFlags = 0;
						bufferDesc.StructureByteStride = 0;
						subresourceData.pSysMem = mesh.vertices.data();
						subresourceData.SysMemPitch = 0;
						subresourceData.SysMemSlicePitch = 0;

						hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.ReleaseAndGetAddressOf());
						_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
					}
					{
						D3D11_BUFFER_DESC bufferDesc = {};
						D3D11_SUBRESOURCE_DATA subresourceData = {};

						bufferDesc.ByteWidth = static_cast<UINT>(sizeof(unsigned int) * mesh.indices.size());
						bufferDesc.Usage = D3D11_USAGE_DEFAULT;
						bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
						bufferDesc.CPUAccessFlags = 0;
						bufferDesc.MiscFlags = 0;
						bufferDesc.StructureByteStride = 0;
						subresourceData.pSysMem = mesh.indices.data();
						subresourceData.SysMemPitch = 0;
						subresourceData.SysMemSlicePitch = 0;
						hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.ReleaseAndGetAddressOf());
						_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
					}
				}
			}
			void CreateShaderResourceViews(ID3D11Device* device, char* mediaDirectory, bool forceSrgb, bool enableCaching)
			{
				std::function<void(const VECTOR4F&, DWORD&)> convert = [&](const VECTOR4F& color, DWORD& RGBA)
				{
					DWORD R = static_cast<BYTE>(color.x * 255);
					DWORD G = static_cast<BYTE>(color.y * 255);
					DWORD B = static_cast<BYTE>(color.z * 255);
					DWORD A = static_cast<BYTE>(color.w * 255);
					RGBA = R | (G << 8) | (B << 16) | (A << 24);
				};
				for (Mesh& mesh : _meshes)
				{
					for (Material& material : mesh.materials)
					{
						if (!material.diffuse.textureFilename.empty())
						{
							char combineResourcePath[256];
							strcpy_s(combineResourcePath, mediaDirectory);
							strcat_s(combineResourcePath, material.diffuse.textureFilename.c_str());
						//	material.diffuse.textureFilename = combineResourcePath;
							Source::Texture::LoadTextureFromFile(device, combineResourcePath, material.diffuse.shaderResourceView.ReleaseAndGetAddressOf(), forceSrgb);
						}
						else
						{
							DWORD RGBA = 0;
							convert(material.diffuse.color, RGBA);
							Source::Texture::MakeDummyTexture(device, material.diffuse.shaderResourceView.ReleaseAndGetAddressOf());
						}
						if (!material.ambient.textureFilename.empty())
						{
							char combineResourcePath[256];
							strcpy_s(combineResourcePath, mediaDirectory);
							strcat_s(combineResourcePath, material.ambient.textureFilename.c_str());
						//	material.ambient.textureFilename = combineResourcePath;
							Source::Texture::LoadTextureFromFile(device, combineResourcePath, material.ambient.shaderResourceView.ReleaseAndGetAddressOf(), forceSrgb);
						}
						else
						{
							DWORD RGBA = 0;
							convert(material.ambient.color, RGBA);
							Source::Texture::MakeDummyTexture(device, material.ambient.shaderResourceView.ReleaseAndGetAddressOf());
						}
						if (!material.specular.textureFilename.empty())
						{
							char combineResourcePath[256];
							strcpy_s(combineResourcePath, mediaDirectory);
							strcat_s(combineResourcePath, material.specular.textureFilename.c_str());
						//	material.specular.textureFilename = combineResourcePath;
							Source::Texture::LoadTextureFromFile(device, combineResourcePath, material.specular.shaderResourceView.ReleaseAndGetAddressOf(), forceSrgb);
						}
						else
						{
							DWORD RGBA = 0;
							convert(material.specular.color, RGBA);
							Source::Texture::MakeDummyTexture(device, material.specular.shaderResourceView.ReleaseAndGetAddressOf());
						}
						if (!material.normalMap.textureFilename.empty())
						{
							char combineResourcePath[256];
							strcpy_s(combineResourcePath, mediaDirectory);
							strcat_s(combineResourcePath, material.normalMap.textureFilename.c_str());
						//	material.normalMap.textureFilename = combineResourcePath;
							Source::Texture::LoadTextureFromFile(device, combineResourcePath, material.normalMap.shaderResourceView.ReleaseAndGetAddressOf(), false/*force_srgb*/);
						}
						else
						{
							Source::Texture::MakeDummyTexture(device, material.normalMap.shaderResourceView.ReleaseAndGetAddressOf());
						}
						if (!material.bump.textureFilename.empty())
						{
							char combineResourcePath[256];
							strcpy_s(combineResourcePath, mediaDirectory);
							strcat_s(combineResourcePath, material.bump.textureFilename.c_str());
						//	material.bump.textureFilename = combineResourcePath;
							Source::Texture::LoadTextureFromFile(device, combineResourcePath, material.bump.shaderResourceView.ReleaseAndGetAddressOf(), false/*force_srgb*/);
						}
						else
						{
							Source::Texture::MakeDummyTexture(device, material.bump.shaderResourceView.ReleaseAndGetAddressOf());
						}
					}
				}
			}
			void AddAnimation(const char* fbxFilename, int animationSamplingRate)
			{
				// Create the FBX SDK manager
				FbxManager* manager = FbxManager::Create();

				// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h.
				manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

				// Create an importer.
				FbxImporter* importer = FbxImporter::Create(manager, "");

				// Initialize the importer.
				bool importStatus = false;
				importStatus = importer->Initialize(fbxFilename, -1, manager->GetIOSettings());
				_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

				// Create a new scene so it can be populated by the imported file.
				FbxScene* scene = FbxScene::Create(manager, "");

				// Import the contents of the file into the scene.
				importStatus = importer->Import(scene);
				_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

				std::vector<AnimationTake> addedAnimationTakes;
				FetchAnimationTakes(scene, addedAnimationTakes, animationSamplingRate);

				std::function<void(FbxNode*, FbxNode*, std::vector<Bone>&, Skeletal&, FbxTime)> traverse =
					[&traverse](FbxNode* node, FbxNode* parentNode, std::vector<Bone>& offsetTransforms, Skeletal& skeletal, FbxTime time)
				{
					FbxNodeAttribute* fbxNodeAttribute = node->GetNodeAttribute();
					if (fbxNodeAttribute)
					{
						switch (fbxNodeAttribute->GetAttributeType())
						{
						case FbxNodeAttribute::eMesh:
							break;
						case FbxNodeAttribute::eSkeleton:
							//'cluster_global_current_position' trnasforms from bone space to global space
							FbxAMatrix clusterGlobalCurrentPosition;
							clusterGlobalCurrentPosition = node->EvaluateGlobalTransform(time);

							FLOAT4X4 transform;
							transform = FbxAMatrixToFloat4x4(clusterGlobalCurrentPosition);

							std::string name = node->GetName();
							int boneIndex;
							int numberOfBones = static_cast<int>(offsetTransforms.size());
							for (boneIndex = 0; boneIndex < numberOfBones; ++boneIndex)
							{
								if (name == offsetTransforms.at(boneIndex).name)
								{
									skeletal.bones.at(boneIndex).name = name;
									skeletal.bones.at(boneIndex).transform = transform;

									// make 'transform_to_parent' that trnasforms from bone space to parent bone space
									skeletal.bones.at(boneIndex).transformToParent = FLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
									if (parentNode)
									{
										FbxAMatrix parentClusterGlobalCurrentPosition;
										parentClusterGlobalCurrentPosition = parentNode->EvaluateGlobalTransform(time);
										skeletal.bones.at(boneIndex).transformToParent = FbxAMatrixToFloat4x4(parentClusterGlobalCurrentPosition.Inverse() * clusterGlobalCurrentPosition);
									}
									break;
								}
							}
							//_ASSERT_EXPR_A(bone_index < number_of_bones, "Not found the bone");
							break;
						}
					}
					for (int i = 0; i < node->GetChildCount(); i++)
					{
						traverse(node->GetChild(i), node, offsetTransforms, skeletal, time);
					}
				};

				for (AnimationTake& animationTake : addedAnimationTakes)
				{
					FbxAnimStack* animationStack = scene->FindMember<FbxAnimStack>(animationTake.name.c_str());
					_ASSERT_EXPR_A(animationStack, "This is a problem. The anim stack should be found in the scene!");
					scene->SetCurrentAnimationStack(animationStack);

					for (Mesh& mesh : _meshes)
					{
						int numberOfBones = static_cast<int>(mesh.offsetTransforms.size());

						float samplingTime = 1.0f / animationTake.samplingRate;
						Animation animation;
						animation.name = animationTake.name;

						FbxTime startTime;
						FbxTime stopTime;
						FbxTakeInfo* takeInfo = scene->GetTakeInfo(animationTake.name.c_str());
						if (takeInfo)
						{
							startTime = takeInfo->mLocalTimeSpan.GetStart();
							stopTime = takeInfo->mLocalTimeSpan.GetStop();
						}
						else
						{
							FbxTimeSpan timeLineTimeSpan;
							scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timeLineTimeSpan);

							startTime = timeLineTimeSpan.GetStart();
							stopTime = timeLineTimeSpan.GetStop();
						}

						FbxTime::EMode timeMode = scene->GetGlobalSettings().GetTimeMode();
						FbxTime smaplingStep;
						smaplingStep.SetTime(0, 0, 1, 0, 0, timeMode);
						smaplingStep = static_cast<FbxLongLong>(smaplingStep.Get() * samplingTime);
						for (FbxTime currentTime = startTime; currentTime < stopTime; currentTime += smaplingStep)
						{
							Skeletal skeletal;
							skeletal.bones.resize(numberOfBones);
							traverse(scene->GetRootNode(), 0, mesh.offsetTransforms, skeletal, currentTime);
							animation.skeletalTransform.push_back(skeletal);
						}
						_ASSERT_EXPR_A(animationTake.numberOfKeyframes == animation.skeletalTransform.size(), "There's no consistency");
						mesh.animations.push_back(animation);
					}
				}
				for (AnimationTake& addedAnimationTake : addedAnimationTakes)
				{
					_animationTakes.push_back(addedAnimationTake);
				}

				manager->Destroy();
			}

			void SaveForBinary(const char* fbxFilename)
			{
				std::ofstream ofs;
				ofs.open((std::string(fbxFilename) + ".bin").c_str(), std::ios::binary);
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(*this);
			}
		public:
			std::vector<Mesh> _meshes;
			std::vector<AnimationTake> _animationTakes;
			HierarchyNode _rootNode;

			FLOAT4X4  axisSystemTransform = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
			};
			template<class T>
			void serialize(T& archive)
			{
				archive
				(
					_meshes, _animationTakes, _rootNode, axisSystemTransform
					);
			}
		private:
			void CreateFromFbx(const char* fbxFilename, int animationSamplingRate)

			{
				// Create the FBX SDK manager
				FbxManager* manager = FbxManager::Create();

				// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h.
				manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

				// Create an importer.
				FbxImporter* importer = FbxImporter::Create(manager, "");

				// Initialize the importer.
				bool importStatus = false;
				importStatus = importer->Initialize(fbxFilename, -1, manager->GetIOSettings());
				_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

				// Create a new scene so it can be populated by the imported file.
				FbxScene* scene = FbxScene::Create(manager, "");

				// Import the contents of the file into the scene.
				importStatus = importer->Import(scene);
				_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

				// Convert axis system (to Y-UP Z-FRONT LHS)
				FbxAxisSystem axisSytemReference = scene->GetGlobalSettings().GetAxisSystem();
				FbxAxisSystem::ECoordSystem coordSystem = axisSytemReference.GetCoorSystem();

				int upVectorSign = 1;
				FbxAxisSystem::EUpVector upVector = axisSytemReference.GetUpVector(upVectorSign);
				_ASSERT_EXPR(upVector == FbxAxisSystem::eYAxis || upVector == FbxAxisSystem::eZAxis, L"Not support X-Up axis system");
				if (upVector == FbxAxisSystem::eYAxis)
				{
					axisSystemTransform = FLOAT4X4(
						1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1
						);
					if (coordSystem == FbxAxisSystem::eRightHanded)
					{
						axisSystemTransform._11 = -1;
					}
					else if (coordSystem == FbxAxisSystem::eLeftHanded)
					{
					}
				}
				else if (upVector == FbxAxisSystem::eZAxis)
				{
					axisSystemTransform = FLOAT4X4(
						1, 0, 0, 0,
						0, 0, 1, 0,
						0, 1, 0, 0,
						0, 0, 0, 1
						);
					if (coordSystem == FbxAxisSystem::eRightHanded)
					{
						axisSystemTransform._11 = -1;
						axisSystemTransform._23 = -1;
					}
					else if (coordSystem == FbxAxisSystem::eLeftHanded)
					{
						axisSystemTransform._23 = -1;
					}
				}

				fbxsdk::FbxGeometryConverter geometryConverter(manager);
				geometryConverter.Triangulate(scene, /*replace*/true);

				if (animationSamplingRate > -1)
				{
					FetchAnimationTakes(scene, _animationTakes, animationSamplingRate);
				}


				// Fetch node attributes and materials under this node recursively. Currently only mesh.
				std::vector<FbxNode*> fetchedMeshes;
				std::function<void(FbxNode*)> traverse = [&](FbxNode* node)
				{
					FbxNodeAttribute* fbxNodeAttribute = node->GetNodeAttribute();
					if (fbxNodeAttribute)
					{
						switch (fbxNodeAttribute->GetAttributeType())
						{
						case FbxNodeAttribute::eMesh:
							fetchedMeshes.push_back(node);
							break;
						}
					}
					for (int i = 0; i < node->GetChildCount(); i++)
					{
						traverse(node->GetChild(i));
					}
				};
				traverse(scene->GetRootNode());

				_meshes.resize(fetchedMeshes.size());
				for (size_t i = 0; i < fetchedMeshes.size(); i++)
				{
					FbxMesh* fbxMesh = fetchedMeshes.at(i)->GetMesh();


					Mesh& mesh = _meshes.at(i);
					mesh.name = fbxMesh->GetNode()->GetName();

					FbxAMatrix globalTransform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
					mesh.globalTransform = FbxAMatrixToFloat4x4(globalTransform);

					FetchMaterials(fbxMesh, mesh.materials);

					std::vector<Vertex> vertices;
					FetchVerticesAndIndices(fbxMesh, vertices, mesh.indices, mesh.materials, mesh.faces, mesh.globalTransform);

					for (const Vertex& v : vertices)
					{
						mesh.vertices.push_back(v);
						mesh.boundingBox.max.x = std::max<float>(v.position.x, mesh.boundingBox.max.x);
						mesh.boundingBox.max.y = std::max<float>(v.position.y, mesh.boundingBox.max.y);
						mesh.boundingBox.max.z = std::max<float>(v.position.z, mesh.boundingBox.max.z);
						mesh.boundingBox.min.x = std::min<float>(v.position.x, mesh.boundingBox.min.x);
						mesh.boundingBox.min.y = std::min<float>(v.position.y, mesh.boundingBox.min.y);
						mesh.boundingBox.min.z = std::min<float>(v.position.z, mesh.boundingBox.min.z);

					}

					FetchOffsetMatrices(fbxMesh, mesh.offsetTransforms);
					for (const AnimationTake& animationTake : _animationTakes)
					{
						Animation animation;
						FetchAnimation(fbxMesh, animationTake, animation);
						mesh.animations.push_back(animation);
					}
				}

				std::function<void(FbxNode*, HierarchyNode&)> traverseHierarchy = [&traverseHierarchy](FbxNode* fbx_node, HierarchyNode& node)
				{
					if (fbx_node)
					{
						FbxNodeAttribute* fbxNodeAttribute = fbx_node->GetNodeAttribute();

						node.attribute = fbxNodeAttribute ? fbxNodeAttribute->GetAttributeType() : FbxNodeAttribute::eUnknown;
						node.name = fbx_node->GetName();
						if (fbx_node->GetParent())
						{
							node.parentName = fbx_node->GetParent()->GetName();
						}

						int numberOfChildren = fbx_node->GetChildCount();
						node.chirdlen.resize(numberOfChildren);
						for (int i = 0; i < numberOfChildren; i++)
						{
							traverseHierarchy(fbx_node->GetChild(i), node.chirdlen.at(i));
						}
					}
				};
				traverseHierarchy(scene->GetRootNode(), _rootNode);

				manager->Destroy();

			}

		};
	}
}