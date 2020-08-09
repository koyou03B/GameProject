#include "BlendingJob.h"

namespace Source
{
	namespace Blend
	{

		bool AnimationBlendingJob::BlendingLayers(const Source::ModelResource::HierarchyNode* parent,
			std::vector<Layer*>& blendLayer,const  FLOAT4X4& parentTransform,int meshNum)
		{
			for (const  Source::ModelResource::HierarchyNode& node : parent->chirdlen)
			{
				if (node.attribute == FbxNodeAttribute::eSkeleton)
				{
					const auto boneSize = blendLayer[0]->transform[meshNum]->size();

					if (boneSize == 0)
					{
						BlendingLayers(&node, blendLayer, FLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), meshNum);
						continue;
					}

					int boneNum;
					for (boneNum = 0; boneNum < static_cast<int>(boneSize - 1u);)
					{
						if (node.name == _boneNames->at(meshNum).at(boneNum))
							break;
						++boneNum;
					}

					FLOAT4X4 blendedTransform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
					{
						const auto& boneTransforms1 = blendLayer[0]->transform[meshNum];
						const auto& boneTransforms2 = blendLayer[1]->transform[meshNum];

						auto& boneTransform1 = boneTransforms1->at(boneNum);
						auto& boneTransform2 = boneTransforms2->at(boneNum);
						auto& outTransform = _output->at(meshNum).at(boneNum);

						DirectX::XMVECTOR Q[2];
						Q[0] = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&boneTransform1));
						Q[1] = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&boneTransform2));
						float weight = blendLayer[1]->weight;
						FLOAT4X4 transform;
						DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionNormalize(DirectX::XMQuaternionSlerp(Q[0], Q[1], weight))));
	
						transform.m[3][0] = boneTransform1.m[3][0] * blendLayer[0]->weight + boneTransform2.m[3][0] * blendLayer[1]->weight;
						transform.m[3][1] = boneTransform1.m[3][1] * blendLayer[0]->weight + boneTransform2.m[3][1] * blendLayer[1]->weight;
						transform.m[3][2] = boneTransform1.m[3][2] * blendLayer[0]->weight + boneTransform2.m[3][2] * blendLayer[1]->weight;

						transform.m[0][3] = 0;
						transform.m[1][3] = 0;
						transform.m[2][3] = 0;
						transform.m[3][3] = 1;

						DirectX::XMStoreFloat4x4(&blendedTransform, DirectX::XMLoadFloat4x4(&transform));
						outTransform = blendedTransform;
					}

					FLOAT4X4 blendedTransformToParent(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
					{
						const auto& boneTransformToParents1 = blendLayer[0]->transformToParent[meshNum];
						const auto& boneTransformToParents2 = blendLayer[1]->transformToParent[meshNum];
						auto& boneTransformToParent1 = boneTransformToParents1->at(boneNum);
						auto& boneTransformToParent2 = boneTransformToParents2->at(boneNum);
						auto& outTransformToParent = _outputToParent->at(meshNum).at(boneNum);

						DirectX::XMVECTOR Q[2];
						Q[0] = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&boneTransformToParent1));
						Q[1] = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&boneTransformToParent2));
						float weight = blendLayer[1]->weight;
						FLOAT4X4 transformToParent;
						DirectX::XMStoreFloat4x4(&transformToParent, DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionNormalize(DirectX::XMQuaternionSlerp(Q[0], Q[1], weight))));

						transformToParent.m[3][0] = boneTransformToParent1.m[3][0] * blendLayer[0]->weight + boneTransformToParent2.m[3][0] * blendLayer[1]->weight;
						transformToParent.m[3][1] = boneTransformToParent1.m[3][1] * blendLayer[0]->weight + boneTransformToParent2.m[3][1] * blendLayer[1]->weight;
						transformToParent.m[3][2] = boneTransformToParent1.m[3][2] * blendLayer[0]->weight + boneTransformToParent2.m[3][2] * blendLayer[1]->weight;

						transformToParent.m[0][3] = 0;
						transformToParent.m[1][3] = 0;
						transformToParent.m[2][3] = 0;
						transformToParent.m[3][3] = 1;

						DirectX::XMStoreFloat4x4(&blendedTransformToParent, DirectX::XMLoadFloat4x4(&transformToParent) * DirectX::XMLoadFloat4x4(&parentTransform));
						outTransformToParent = blendedTransformToParent;

					}
					BlendingLayers(&node, blendLayer, blendedTransformToParent, meshNum);
				}

				else
				{
					BlendingLayers(&node, blendLayer, FLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), meshNum);
				}
			}

			return true;
		}

		bool AnimationBlendingJob::Run()
		{
			if (!Validate())
				return false;

			std::vector<Layer*> blendLayer;
			for (auto& layer : *_layer)
			{
				if (layer.weight <= 0.0f)
					continue;
				blendLayer.emplace_back(&layer);
			}

			for (int meshNum = 0; meshNum < static_cast<int>(blendLayer[0]->transform.size()); ++meshNum)
			{
				FLOAT4X4 parentTransform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

				if (blendLayer.size() == 1)
				{
					const int boneSize = static_cast<int>(blendLayer[0]->transform[meshNum]->size());
					const auto& boneTransforms = blendLayer[0]->transform[meshNum];
					const auto& boneTransformsToParents = blendLayer[0]->transformToParent[meshNum];

					for (int boneNum = 0; boneNum < boneSize; ++boneNum)
					{
						auto& boneTransform1 = boneTransforms->at(boneNum);
						auto& outTransform = _output->at(meshNum).at(boneNum);
						outTransform = boneTransform1;

						auto& boneTransformToParents1 = boneTransformsToParents->at(boneNum);
						auto& outToParentTransform = _outputToParent->at(meshNum).at(boneNum);
						outToParentTransform = boneTransformToParents1;
					}
				}
				else
				{
					BlendingLayers(_node, blendLayer, parentTransform, meshNum);
				}
			}
		
			return true;
		}


		bool PartialAnimationBlendingJob::BlendingLayers(const Source::ModelResource::HierarchyNode* parent,
			std::vector<Layer*>& blendLayer, const  FLOAT4X4& parentTransform, int meshNum)
		{
			for (const Source::ModelResource::HierarchyNode& node : parent->chirdlen)
			{
				if (node.attribute == FbxNodeAttribute::eSkeleton)
				{
					const auto boneSize = blendLayer[0]->transform[meshNum]->size();

					if (boneSize == 0)
					{
						BlendingLayers(&node, blendLayer, FLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), meshNum);
						continue;
					}

					int boneNum;
					for (boneNum = 0; boneNum < static_cast<int>(boneSize - 1u);)
					{
						if (node.name == _boneNames->at(meshNum).at(boneNum))
							break;
						++boneNum;
					}

					FLOAT4X4 blendedTransform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
					{
						const auto& lowerBodyBoneTransforms = blendLayer[0]->transform[meshNum];
						const auto& upperBodyBoneTransforms = blendLayer[1]->transform[meshNum];
						auto& lowerBodyBoneTransform = lowerBodyBoneTransforms->at(boneNum);
						auto& upperBodyBoneTransform = upperBodyBoneTransforms->at(boneNum);
						auto& outTransform = _output->at(meshNum).at(boneNum);

						auto lowerBodyBlendWeight = blendLayer[0]->partialBlendWeight.at(meshNum).at(boneNum);
						auto upperBodyBlendWeight = blendLayer[1]->partialBlendWeight.at(meshNum).at(boneNum);

						DirectX::XMVECTOR Q[2];
						Q[0] = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&lowerBodyBoneTransform));
						Q[1] = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&upperBodyBoneTransform));
	
						float weight = blendLayer[1]->weight * upperBodyBlendWeight;

						FLOAT4X4 transform;
						DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionNormalize(DirectX::XMQuaternionSlerp(Q[0], Q[1], weight))));

						transform.m[3][0] = lowerBodyBoneTransform.m[3][0] * lowerBodyBlendWeight + upperBodyBoneTransform.m[3][0] *  upperBodyBlendWeight;
						transform.m[3][1] = lowerBodyBoneTransform.m[3][1] * lowerBodyBlendWeight + upperBodyBoneTransform.m[3][1] *  upperBodyBlendWeight;
						transform.m[3][2] = lowerBodyBoneTransform.m[3][2] * lowerBodyBlendWeight + upperBodyBoneTransform.m[3][2] *  upperBodyBlendWeight;

						transform.m[0][3] = 0;
						transform.m[1][3] = 0;
						transform.m[2][3] = 0;
						transform.m[3][3] = 1;

						DirectX::XMStoreFloat4x4(&blendedTransform, DirectX::XMLoadFloat4x4(&transform));
						outTransform = blendedTransform;
					}

					FLOAT4X4 blendedTransformToParent(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
					{
						const auto& lowerBodyBoneTransformToParents = blendLayer[0]->transformToParent[meshNum];
						const auto& upperBodyBoneTransformToParents = blendLayer[1]->transformToParent[meshNum];
						auto& lowerBodyBoneTransformToParent = lowerBodyBoneTransformToParents->at(boneNum);
						auto& upperBodyBoneTransformToParent = upperBodyBoneTransformToParents->at(boneNum);
						auto& outTransformToParent = _outputToParent->at(meshNum).at(boneNum);

						auto lowerBodyBlendWeight = blendLayer[0]->partialBlendWeight.at(meshNum).at(boneNum);
						auto upperBodyBlendWeight = blendLayer[1]->partialBlendWeight.at(meshNum).at(boneNum);

						DirectX::XMVECTOR Q[2];
						Q[0] = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&lowerBodyBoneTransformToParent));
						Q[1] = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&upperBodyBoneTransformToParent));

						float weight = blendLayer[1]->weight * upperBodyBlendWeight;

						FLOAT4X4 transformToParent;
						DirectX::XMStoreFloat4x4(&transformToParent, DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionNormalize(DirectX::XMQuaternionSlerp(Q[0], Q[1], weight))));

						transformToParent.m[3][0] = lowerBodyBoneTransformToParent.m[3][0] * lowerBodyBlendWeight + upperBodyBoneTransformToParent.m[3][0] * upperBodyBlendWeight;
						transformToParent.m[3][1] = lowerBodyBoneTransformToParent.m[3][1] * lowerBodyBlendWeight + upperBodyBoneTransformToParent.m[3][1] * upperBodyBlendWeight;
						transformToParent.m[3][2] = lowerBodyBoneTransformToParent.m[3][2] * lowerBodyBlendWeight + upperBodyBoneTransformToParent.m[3][2] * upperBodyBlendWeight;

						transformToParent.m[0][3] = 0;
						transformToParent.m[1][3] = 0;
						transformToParent.m[2][3] = 0;
						transformToParent.m[3][3] = 1;

						DirectX::XMStoreFloat4x4(&blendedTransformToParent, DirectX::XMLoadFloat4x4(&transformToParent) * DirectX::XMLoadFloat4x4(&parentTransform));
						outTransformToParent = blendedTransformToParent;

					}
					BlendingLayers(&node, blendLayer, blendedTransformToParent, meshNum);
				}

				else
				{
					BlendingLayers(&node, blendLayer, FLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), meshNum);
				}
			}

			return true;
		}

		bool PartialAnimationBlendingJob::Run()

		{
			if (!Validate())
				return false;

			std::vector<Layer*> blendLayer;
			for (auto& layer : *_layer)
			{
				blendLayer.emplace_back(&layer);
			}

			for (int meshNum = 0; meshNum < static_cast<int>(blendLayer[0]->transform.size()); ++meshNum)
			{
				FLOAT4X4 parentTransform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

				if (blendLayer.size() == 1)
				{
					const int boneSize = static_cast<int>(blendLayer[0]->transform[meshNum]->size());
					const auto& boneTransforms = blendLayer[0]->transform[meshNum];
					const auto& boneTransformsToParents = blendLayer[0]->transformToParent[meshNum];

					for (int boneNum = 0; boneNum < boneSize; ++boneNum)
					{
						auto& boneTransform1 = boneTransforms->at(boneNum);
						auto& outTransform = _output->at(meshNum).at(boneNum);
						outTransform = boneTransform1;

						auto& boneTransformToParents1 = boneTransformsToParents->at(boneNum);
						auto& outToParentTransform = _outputToParent->at(meshNum).at(boneNum);
						outToParentTransform = boneTransformToParents1;
					}
				}
				else
				{
					BlendingLayers(_node, blendLayer, parentTransform, meshNum);
				}
			}

			return true;
		}

	}
}