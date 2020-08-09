#pragma once
#include "ModelResource.h"
#include "Vector.h"

namespace Source
{
	namespace Blend
	{
		class SamplingJob
		{
		public:
			const Source::ModelResource::Animation* _animation;
			float _ratio;
			float _duration;
			std::vector<FLOAT4X4>* _output;
			std::vector<FLOAT4X4>* _outputToParents;

		public:
			SamplingJob() : _animation(nullptr), _ratio(0), _duration(0),
				_output(nullptr), _outputToParents(nullptr){}

			~SamplingJob() = default;

			bool Validate() const;

			bool Run() const;


		};
	}
}