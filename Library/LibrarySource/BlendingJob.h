#pragma once
#include <vector>
#include <queue>
#include "ModelResource.h"
#include "Vector.h"


namespace Source
{
	namespace Blend
	{
		class BlendingJob
		{
		public:
			std::vector<std::vector<std::string>>* _boneNames;
			std::vector<std::vector<FLOAT4X4>>* _output;
			std::vector<std::vector<FLOAT4X4>>* _outputToParent;
			Source::ModelResource::HierarchyNode* _node;

		public:
			BlendingJob() : _boneNames(nullptr), _output(nullptr),
				_outputToParent(nullptr), _node(nullptr) {};

			~BlendingJob() = default;

			virtual bool Validate()  = 0;

			virtual bool Run() = 0;
		};

		class AnimationBlendingJob : public BlendingJob
		{
		public:
			struct Layer
			{
				float weight;
				std::vector<std::vector<FLOAT4X4>*> transform;
				std::vector<std::vector<FLOAT4X4>*> transformToParent;

				Layer() : weight(0) {};
			};
			std::vector<Layer>* _layer;
		public:

			AnimationBlendingJob() : _layer(0) {};
			~AnimationBlendingJob() = default;

			bool Validate() 
			{
				if (_layer != nullptr)
					return true;

				return false;
			};

			bool Run();

			bool BlendingLayers(const Source::ModelResource::HierarchyNode* parent,
				std::vector<Layer*>& blendLayer,
				const FLOAT4X4& parentTransform,
				int meshNum);
		};

		class PartialAnimationBlendingJob : public BlendingJob
		{
		public:
			struct Layer
			{
				float weight;
				std::vector<std::vector<float>> partialBlendWeight;
				std::vector<std::vector<FLOAT4X4>*> transform;
				std::vector<std::vector<FLOAT4X4>*> transformToParent;

				Layer() : weight(0) {};
			};
			std::vector<Layer>* _layer;
		public:

			PartialAnimationBlendingJob() : _layer(0) {};
			~PartialAnimationBlendingJob() = default;

			bool Validate() 
			{
				if (_layer != nullptr)
					return true;

				return false;
			};

			bool Run();

			bool BlendingLayers(const Source::ModelResource::HierarchyNode* parent,
				std::vector<Layer*>& blendLayer,
				const FLOAT4X4& parentTransform,
				int meshNum);
		};
	}
}
