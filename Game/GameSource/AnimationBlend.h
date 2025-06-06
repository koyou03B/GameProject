#pragma once
#include <vector>
#include <queue>
#include <string>

#include "LibrarySource/Vector.h"
#include "LibrarySource/SkinnedMesh.h"


class AnimationBlend
{
public:
	using Sampler = Source::ModelResource::AnimationTake;

	float _blendRatio;
	float _animationSpeed = 1.0f;
	std::vector<std::vector<FLOAT4X4>> _blendLocals;
	std::vector<std::vector<FLOAT4X4>> _blendLocalsToParents;

public:
	AnimationBlend() :_blendRatio(0) {}
	~AnimationBlend() = default;

	virtual bool Init(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model);

	virtual void Update(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model, float& elapsedTime);

	void UpdateRuntimeParameters();

	bool SearchSampler(int animNo)
	{
		int count = static_cast<int>(m_samplers.size());
		for (int i = 0; i < count; ++i)
		{
			if (m_samplers[i].first == animNo)
				return true;
		}
		return false;
	}

	void AddSampler(int animNum, std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model)
	{
		int count = static_cast<int>(m_samplers.size());
		for (int i = 0; i < count; ++i)
		{
			if (m_samplers[i].first == animNum)
				return;
		}
		auto& anim = model->_resource->_animationTakes[animNum];
		m_samplers.push_back(std::make_pair(animNum, &anim));
	}

	void ChangeSampler(int samplerNum, int animNum, std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model)
	{
		auto& anim = model->_resource->_animationTakes[animNum];
		m_samplers[samplerNum] = std::make_pair(animNum, &anim);
	}

	void ReleaseSampler(int samplerNum)
	{
		int samplerCount = static_cast<int>(m_samplers.size());
		if (samplerCount >= 2)
		{
			ResetAnimationSampler(samplerNum);
			m_samplers.erase(m_samplers.begin() + samplerNum);
		}
	}

	std::vector<std::pair<int, Sampler*>>& GetSampler() { return m_samplers; }

	inline void ResetAnimationSampler(int samplerNum)
	{
		if (samplerNum < static_cast<int>(m_samplers.size()))
			m_samplers[samplerNum].second->animtion.Reset();
	}

	inline void FalseAnimationLoop(int samplerNum)
	{
		m_samplers[samplerNum].second->animtion.SetIsLoop(false);
	}

	inline void TrueAnimationLoop(int samplerNum)
	{
		m_samplers[samplerNum].second->animtion.SetIsLoop(true);
	}

	inline void SetAnimationSpeed(float speed)
	{
		_animationSpeed = speed;
	}

	inline uint32_t GetAnimationTime(int samplerNum)
	{
		return m_animationFrame[samplerNum];
	}

	inline void ResetAnimationFrame()
	{
		int count = static_cast<int>(m_animationFrame.size());
		for (int i = 0; i < count; ++i)
		{
			m_animationFrame[i] = 0;
		}
	}

	void ReleaseAllSampler()
	{
		int count = static_cast<int>(m_samplers.size());
		for (int i = 0; i < count; ++i)
		{
			ResetAnimationSampler(i);
		}
		m_samplers.clear();
	}

	std::vector<std::vector<std::string>>& GetBoneName() { return m_boneNames; }

protected:
	std::vector<std::pair<int, Sampler*>> m_samplers;
	std::vector<std::vector<std::string>> m_boneNames;
	std::vector<uint32_t> m_animationFrame;
};