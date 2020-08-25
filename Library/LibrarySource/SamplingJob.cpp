#include "SamplingJob.h"
#include "Function.h"

namespace Source
{
	namespace Blend
	{
		bool SamplingJob::Validate() const
		{
			bool valid = true;

			if (!_animation)
				return false;


			return valid;
		}
		bool SamplingJob::Run() const
		{
			if (!Validate())
				return false;

			const float animRatio = Source::Math::Clamp(0.0f, _ratio, 1.0f);
			*_animationFrame = static_cast<uint32_t>(animRatio * _duration * 60.0f);

			auto& skeltal = _animation->skeletalTransform[*_animationFrame];
		
			for (int i = 0; i < static_cast<int>(_output->size()); ++i)
			{
				auto& bone = skeltal.bones[i];
				DirectX::XMMATRIX M;
				M = DirectX::XMLoadFloat4x4(&bone.transform);
				DirectX::XMStoreFloat4x4(&_output->at(i), M);

				M = DirectX::XMLoadFloat4x4(&bone.transformToParent);
				DirectX::XMStoreFloat4x4(&_outputToParents->at(i), M);

			}

			return true;
		}
	}
}