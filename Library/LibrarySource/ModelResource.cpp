#include "ModelResource.h"
#include "Function.h"
// FbxDouble2 Å® VEC2
inline VECTOR2F FbxDouble2ToFloat2(const FbxDouble2& fbx_value)
{
	return VECTOR2F(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1])
	);
}

// FbxDouble3 Å® VEC3
inline VECTOR3F FbxDouble3ToFloat3(const FbxDouble3& fbx_value)
{
	return VECTOR3F(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2])
	);
}

// FbxDouble4 Å® VEC3
inline VECTOR3F FbxDouble4ToFloat3(const FbxDouble4& fbx_value)
{
	return VECTOR3F(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2])
	);
}

// FbxDouble4 Å® VEC4
inline VECTOR4F FbxDouble4ToFloat4(const FbxDouble4& fbx_value)
{
	return VECTOR4F(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2]),
		static_cast<float>(fbx_value[3])
	);
}


struct BoneInfluence
{
	int index;	// index of bone
	float weight; // weight of bone
};
struct BoneInfluencesPerControlPoint : public std::vector<BoneInfluence>
{
	float totalWeight = 0;
};

void FetchBoneInfluences(const FbxMesh* fbxMesh, std::vector<BoneInfluencesPerControlPoint>& influences)
{
	const int numberOfControlPoints = fbxMesh->GetControlPointsCount();
	influences.resize(numberOfControlPoints);

	const int numberOfDeformers = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int indexOfDeformer = 0; indexOfDeformer < numberOfDeformers; ++indexOfDeformer)
	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(indexOfDeformer, FbxDeformer::eSkin));

		const int numberOfClusters = skin->GetClusterCount();
		for (int indexOfCluster = 0; indexOfCluster < numberOfClusters; ++indexOfCluster)
		{
			FbxCluster* cluster = skin->GetCluster(indexOfCluster);

			const int	numberOfControlPointIndices = cluster->GetControlPointIndicesCount();
			const int* arrayOfControlPointIndices = cluster->GetControlPointIndices();
			const double* arrayOfControlPointWeights = cluster->GetControlPointWeights();


			for (int i = 0; i < numberOfControlPointIndices; ++i)
			{
				BoneInfluencesPerControlPoint& influencesPerControlPoint = influences.at(arrayOfControlPointIndices[i]);
				BoneInfluence influence;
				influence.index = indexOfCluster;
				influence.weight = static_cast<float>(arrayOfControlPointWeights[i]);
				influencesPerControlPoint.push_back(influence);
				influencesPerControlPoint.totalWeight += influence.weight;
			}
		}
	}
}

void SetBoneInfluencesToVertex(Source::ModelResource::Vertex& vertex, BoneInfluencesPerControlPoint& influencesPerControlPoint)
{
	for (size_t indexOfInfluence = 0; indexOfInfluence < influencesPerControlPoint.size(); ++indexOfInfluence)
	{
		if (indexOfInfluence < MAX_BONE_INFLUENCES)
		{
			vertex.boneWeights[indexOfInfluence] = influencesPerControlPoint.at(indexOfInfluence).weight / influencesPerControlPoint.totalWeight;
			vertex.boneIndices[indexOfInfluence] = influencesPerControlPoint.at(indexOfInfluence).index;
		}
		else
		{
#if 0
			for (int i = 0; i < MAX_BONE_INFLUENCES; i++)
			{
				if (vertex.bone_weights[i] < influences_per_control_point.at(index_of_influence).weight)
				{
					vertex.bone_weights[i] += influences_per_control_point.at(index_of_influence).weight / influences_per_control_point.total_weight;
					vertex.bone_indices[i] = influences_per_control_point.at(index_of_influence).index;
					break;
				}
			}
#else
			int minIndex = 0;
			float minValue = FLT_MAX;
			for (int i = 0; i < MAX_BONE_INFLUENCES; ++i)
			{
				if (minValue > vertex.boneWeights[i])
				{
					minValue = vertex.boneWeights[i];
					minIndex = i;
				}
			}
			vertex.boneWeights[minIndex] += influencesPerControlPoint.at(indexOfInfluence).weight / influencesPerControlPoint.totalWeight;
			vertex.boneIndices[minIndex] = influencesPerControlPoint.at(indexOfInfluence).index;
#endif
		}
	}
}

void FetchBoneMatrices(FbxMesh* fbxMesh, Source::ModelResource::Skeletal& skeletal, FbxTime time)
{
	const int numberOfDeformers = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int indexOfDeformer = 0; indexOfDeformer < numberOfDeformers; ++indexOfDeformer)
	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(indexOfDeformer, FbxDeformer::eSkin));

		FbxSkin::EType skinningType = skin->GetSkinningType();
		_ASSERT_EXPR(skinningType == FbxSkin::eLinear || skinningType == FbxSkin::eRigid, L"only support FbxSkin::eLinear or FbxSkin::eRigid");

		FbxCluster::ELinkMode linkMode = skin->GetCluster(0)->GetLinkMode();
		_ASSERT_EXPR(linkMode != FbxCluster::eAdditive, L"only support FbxCluster::eNormalize or FbxCluster::eTotalOne");

		const int numberOfClusters = skin->GetClusterCount();
		skeletal.bones.resize(numberOfClusters);
		for (int indexOfCluster = 0; indexOfCluster < numberOfClusters; ++indexOfCluster)
		{
			Source::ModelResource::Bone& bone = skeletal.bones.at(indexOfCluster);

			FbxCluster* cluster = skin->GetCluster(indexOfCluster);
			bone.name = cluster->GetLink()->GetName();

			// 'clusterGlobalCurrentPosition' trnasforms from bone space to global space
			FbxAMatrix clusterGlobalCurrentPosition;
			clusterGlobalCurrentPosition = cluster->GetLink()->EvaluateGlobalTransform(time);
			bone.transform = FbxAMatrixToFloat4x4(clusterGlobalCurrentPosition);

			// make 'bone.transformToParent' that trnasforms from bone space to parent bone space
			bone.transformToParent = FLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
			FbxNode* parentNode = cluster->GetLink()->GetParent();
			if (parentNode)
			{
				FbxAMatrix parentClusterGlobalCurrentPosition;
				parentClusterGlobalCurrentPosition = parentNode->EvaluateGlobalTransform(time);
				bone.transformToParent = FbxAMatrixToFloat4x4(parentClusterGlobalCurrentPosition.Inverse() * clusterGlobalCurrentPosition);
			}
		}
	}
}

void FetchKeyframeMatrices(FbxMesh* fbxMesh, FLOAT4X4& keyframeTransform, FbxTime time)
{
	// 'reference_global_current_position' trnasforms from model space to global space
	FbxAMatrix referenceGlobalCurrentPosition;
	referenceGlobalCurrentPosition = fbxMesh->GetNode()->EvaluateGlobalTransform(time);
	keyframeTransform = FbxAMatrixToFloat4x4(referenceGlobalCurrentPosition);
}

namespace Source
{
	namespace ModelResource
	{
		void ModelResource::FetchAnimationTakes(FbxScene* scene, std::vector<AnimationTake>& animationTakes, int samplingRate/*0:default value*/)
		{
			FbxArray<FbxString*> arrayOfAnimationStackNames;
			scene->FillAnimStackNameArray(arrayOfAnimationStackNames);
			const int numberOfAnimationStacks = arrayOfAnimationStackNames.GetCount();
			for (int indexOfAnimationStack = 0; indexOfAnimationStack < numberOfAnimationStacks; indexOfAnimationStack++)
			{
				AnimationTake animationTake;
				animationTake.name = arrayOfAnimationStackNames[indexOfAnimationStack]->Buffer();

				FbxAnimStack* animationStack = scene->FindMember<FbxAnimStack>(arrayOfAnimationStackNames[indexOfAnimationStack]->Buffer());
				_ASSERT_EXPR_A(animationStack, "This is a problem. The anim stack should be found in the scene!");
				scene->SetCurrentAnimationStack(animationStack);

				FbxTime::EMode timeMode = scene->GetGlobalSettings().GetTimeMode();
				FbxTime frameTime;
				frameTime.SetTime(0, 0, 0, 1, 0, timeMode);
				samplingRate = samplingRate > 0 ? samplingRate : static_cast<unsigned int>(frameTime.GetFrameRate(timeMode));
				animationTake.samplingRate = static_cast<float>(samplingRate);

				animationTake.period = 1.0f / animationTake.samplingRate;

				FbxTime smaplingStep;
				smaplingStep.SetTime(0, 0, 1, 0, 0, timeMode);
				smaplingStep = static_cast<FbxLongLong>(smaplingStep.Get() * (1.0f / samplingRate));

				FbxTakeInfo* takeInfo = scene->GetTakeInfo(*(arrayOfAnimationStackNames[indexOfAnimationStack]));
				FbxTime startTime;
				FbxTime stopTime;
				if (takeInfo)
				{
					startTime = takeInfo->mLocalTimeSpan.GetStart();
					stopTime = takeInfo->mLocalTimeSpan.GetStop();

					animationTake.start = static_cast<float>(takeInfo->mLocalTimeSpan.GetStart().GetSecondDouble());
					animationTake.end = static_cast<float>(takeInfo->mLocalTimeSpan.GetStop().GetSecondDouble());

				}
				else
				{
					FbxTimeSpan timeLineTimeSpan;
					scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timeLineTimeSpan);
					startTime = timeLineTimeSpan.GetStart();
					stopTime = timeLineTimeSpan.GetStop();
					animationTake.start = static_cast<float>(timeLineTimeSpan.GetStart().GetSecondDouble());
					animationTake.end   = static_cast<float>(timeLineTimeSpan.GetStop().GetSecondDouble());
				}

				if (animationTake.end > animationTake.start)
					animationTake.duration = animationTake.end - animationTake.start;
				else
					animationTake.duration = 1.0f;

				animationTake.numberOfKeyframes = static_cast<int>(((stopTime - startTime) / smaplingStep).Get()) + 1;
				animationTakes.push_back(animationTake);
			}
			for (int i = 0; i < numberOfAnimationStacks; i++)
			{
				delete arrayOfAnimationStackNames[i];
			}
		}


		ModelResource::Vertex FetchVertex(FbxMesh* fbxMesh, FbxStringList& uvNames, int indexOfPolygon, int indexOfEdge)

		{
			ModelResource::Vertex vertex;

			bool hasNormal = fbxMesh->GetElementNormalCount() > 0;
			bool hasUV = fbxMesh->GetElementUVCount() > 0;

			const FbxVector4* arrayOfControlPoints = fbxMesh->GetControlPoints();

			const int indexOfControlPoint = fbxMesh->GetPolygonVertex(indexOfPolygon, indexOfEdge);

			vertex.position.x = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][0]);
			vertex.position.y = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][1]);
			vertex.position.z = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][2]);

			if (hasNormal)
			{
				FbxVector4 normal;
				fbxMesh->GetPolygonVertexNormal(indexOfPolygon, indexOfEdge, normal);
				vertex.normal.x = static_cast<float>(normal[0]);
				vertex.normal.y = static_cast<float>(normal[1]);
				vertex.normal.z = static_cast<float>(normal[2]);
			}

			if (hasUV)
			{
				FbxVector2 uv;
				bool unmappedUV;
				fbxMesh->GetPolygonVertexUV(indexOfPolygon, indexOfEdge, uvNames[0], uv, unmappedUV);
				vertex.texcoord.x = static_cast<float>(uv[0]);
				vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
			}

			return vertex;
		}

		void ModelResource::FetchAnimation(FbxMesh* fbxMesh, const AnimationTake& animationTake, Animation& animation)
		{
			_ASSERT_EXPR_A(animationTake.samplingRate > 0, "'animationTake.samplingRate' is an invalid value.");

			FbxScene* scene = fbxMesh->GetScene();
			FbxAnimStack* animationStack = scene->FindMember<FbxAnimStack>(animationTake.name.c_str());
			scene->SetCurrentAnimationStack(animationStack);

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
			smaplingStep = static_cast<FbxLongLong>(smaplingStep.Get() * (1.0f / animationTake.samplingRate));
			for (FbxTime time = startTime; time < stopTime; time += smaplingStep)
			{
				Skeletal skeletal;
				FetchBoneMatrices(fbxMesh, skeletal, time);
				animation.skeletalTransform.push_back(skeletal);

				ActorTransform actor_transform;
				FetchKeyframeMatrices(fbxMesh, actor_transform.transform, time);
				animation.actorTransforms.push_back(actor_transform);
			}
			//_ASSERT_EXPR_A(animationTake.numberOfKeyframes == animation.skeletalTransform.size(), "There's no consistency");
			//_ASSERT_EXPR_A(animationTake.numberOfKeyframes == animation.actorTransforms.size(), "There's no consistency");
			animation.name = animationTake.name;
		}

		void ModelResource::FetchOffsetMatrices(FbxMesh* fbxMesh, std::vector<Bone>& offsetTransforms)
		{
			const int numberOfDeformers = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
			for (int indexOfDeformer = 0; indexOfDeformer < numberOfDeformers; ++indexOfDeformer)
			{
				FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(indexOfDeformer, FbxDeformer::eSkin));

				const int numberOfClusters = skin->GetClusterCount();
				offsetTransforms.resize(numberOfClusters);
				for (int indexOfCluster = 0; indexOfCluster < numberOfClusters; ++indexOfCluster)
				{
					Bone& bone = offsetTransforms.at(indexOfCluster);

					FbxCluster* cluster = skin->GetCluster(indexOfCluster);
					bone.name = cluster->GetLink()->GetName();
					bone.parentName = cluster->GetLink()->GetParent()->GetName();
					
					// 'reference_global_init_position' trnasforms from model space to global space
					FbxAMatrix referenceGlobalInitPosition;
					cluster->GetTransformMatrix(referenceGlobalInitPosition);

					// 'clusterGlobalInitPosition' trnasforms from bone space to global space
					FbxAMatrix clusterGlobalInitPosition;
					cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

					// Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation (translation, rotation and scale), the last row of the matrix represents the translation part of the transformation.
					// make 'bone.transform' that trnasforms from model space to bone space (offset matrix)
					bone.transform = FbxAMatrixToFloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);

					// 'bone.transform_to_parent' is noe used in offset transforms
					bone.transformToParent = FLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
				}
			}
		}

		void ModelResource::FetchMaterials(FbxMesh* fbxMesh, std::vector<Material>& materials)
		{
			// Fetch material properties.
			const int numberOfMaterials = fbxMesh->GetNode()->GetMaterialCount();
			materials.resize(numberOfMaterials > 0 ? numberOfMaterials : 1);
			for (int indexOfMaterial = 0; indexOfMaterial < numberOfMaterials; ++indexOfMaterial)
			{
				Material& material = materials.at(indexOfMaterial);

				const FbxSurfaceMaterial* surfaceMaterial = fbxMesh->GetNode()->GetMaterial(indexOfMaterial);
				material.name = surfaceMaterial->GetName();

				std::function<void(Material::property&, const char*, const char*)> fetchMaterialProperty = [&](Material::property& material, const char* property_name, const char* factor_name)
				{
					const FbxProperty property = surfaceMaterial->FindProperty(property_name);
					const FbxProperty factor = surfaceMaterial->FindProperty(factor_name);
					if (property.IsValid() && factor.IsValid())
					{
						FbxDouble3 color = property.Get<FbxDouble3>();
						double f = factor.Get<FbxDouble>();
						material.color.x = static_cast<float>(color[0]);
						material.color.y = static_cast<float>(color[1]);
						material.color.z = static_cast<float>(color[2]);
						material.color.w = 1.0f;
					}

					if (property.IsValid())
					{
						const int numberOfTextures = property.GetSrcObjectCount<FbxFileTexture>();
						if (numberOfTextures > 0)
						{
							const FbxFileTexture* fileTexture = property.GetSrcObject<FbxFileTexture>();
							if (fileTexture)
							{
								//material.texture_filename = file_texture->GetFileName();
								material.textureFilename = fileTexture->GetRelativeFileName();
							}
						}
					}
				};
				fetchMaterialProperty(material.diffuse, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
				fetchMaterialProperty(material.ambient, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
				fetchMaterialProperty(material.specular, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
				fetchMaterialProperty(material.normalMap, FbxSurfaceMaterial::sNormalMap, FbxSurfaceMaterial::sBumpFactor);
				fetchMaterialProperty(material.bump, FbxSurfaceMaterial::sBump, FbxSurfaceMaterial::sBumpFactor);
			}

			// Count the polygon count of each material
			if (numberOfMaterials > 0)
			{
				// Count the faces of each material
				const int number_of_polygons = fbxMesh->GetPolygonCount();
				for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; ++index_of_polygon)
				{
					const unsigned int material_index = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
					materials.at(material_index).indexCount += 3;
				}

				// Record the offset (how many vertex)
				int offset = 0;
				for (Material& material : materials)
				{
					material.indexStart = offset;
					offset += material.indexCount;
					// This will be used as counter in the following procedures, reset to zero
					material.indexCount = 0;
				}
			}

		}


		void ModelResource::FetchVerticesAndIndices(FbxMesh* fbxMesh, std::vector<ModelResource::Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<ModelResource::Material>& materials,
			std::vector<ModelResource::Face>& faces, FLOAT4X4& globalTransform)
		{
			// How to Work with FBX SDK https://www.gamedev.net/articles/programming/graphics/how-to-work-with-fbx-sdk-r3582

			const int numberOfPolygons = fbxMesh->GetPolygonCount();
			const FbxVector4* arrayOfControlPoints = fbxMesh->GetControlPoints();

			std::vector<BoneInfluencesPerControlPoint> boneInfluences;
			FetchBoneInfluences(fbxMesh, boneInfluences);

			FbxStringList uvNames;
			fbxMesh->GetUVSetNames(uvNames);

			// Rebulid indices array
			unsigned int vertexCount = 0;
			const int numberOfMaterials = fbxMesh->GetNode()->GetMaterialCount();
			indices.resize(numberOfPolygons * 3);
			for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; indexOfPolygon++)
			{
				// The material for current face
				int indexOfMaterial;
				if (numberOfMaterials > 0)
				{
					indexOfMaterial = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);
				}
				else // If fbx_mesh has no material then make one dummy material
				{
					indexOfMaterial = 0;
				}

				// Where should I save the vertex attribute index, according to the material
				Material& material = materials.at(indexOfMaterial);
				const int indexOffset = material.indexStart + material.indexCount;
				Face face;
				face.materialIndex = indexOfMaterial;

				for (int indexOfEdge = 0; indexOfEdge < 3; ++indexOfEdge)
				{
					const int indexOfControlPoint = fbxMesh->GetPolygonVertex(indexOfPolygon, indexOfEdge);
					indices.at(indexOffset + indexOfEdge) = static_cast<unsigned int>(vertexCount);
					vertexCount += 1;

					ModelResource::Vertex vertex;
					vertex = FetchVertex(fbxMesh, uvNames, indexOfPolygon, indexOfEdge);

					//ï°êîÉÅÉbÉVÉÖ
					DirectX::XMVECTOR localPosition = DirectX::XMLoadFloat3(&vertex.position);
					DirectX::XMVECTOR globalPosition = DirectX::XMVector3TransformCoord(localPosition, DirectX::XMLoadFloat4x4(&globalTransform));
					DirectX::XMStoreFloat3(&face.position[indexOfEdge], globalPosition);


					BoneInfluencesPerControlPoint influencesPerControlPoint = boneInfluences.at(indexOfControlPoint);
					SetBoneInfluencesToVertex(vertex, influencesPerControlPoint);

					vertices.push_back(vertex);
				}
				material.indexCount += 3;
				faces.push_back(face);
			}
		}

	}
}
