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
	std::vector<std::vector<FLOAT4X4>> _blendLocals;
	std::vector<std::vector<FLOAT4X4>> _blendLocalsToParents;

public:
	AnimationBlend() :_blendRatio(0) {}
	~AnimationBlend() = default;

	virtual bool Init(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model);

	virtual void Update(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model,float& elapsedTime);

	void UpdateRuntimeParameters();

	void AddSampler(int animNum, std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model)
	{
		auto& anim = model->_resource->_animationTakes[animNum];
		m_samplers.push_back(std::make_pair(animNum, &anim));
	}
	
	void ChangeSampler(int samplerNum,int animNum, std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model)
	{
		auto& anim = model->_resource->_animationTakes[animNum];
		m_samplers[samplerNum] = std::make_pair(animNum, &anim);
	}

	void ReleaseSampler(int samplerNum)
	{
		m_samplers.erase(m_samplers.begin()+samplerNum);
	}

	void ReleaseAllSampler(int samplerNum)
	{
		m_samplers.clear();
	}

	std::vector<std::vector<std::string>>& GetBoneName() { return m_boneNames; }

protected:
	std::vector<std::pair<int, Sampler*>> m_samplers;
	std::vector<std::vector<std::string>> m_boneNames;
};


