#pragma once
#include <vector>
#include <queue>
#include <string>

#include "..\LibrarySource\EntityComponentSystem.h"
#include "..\LibrarySource\Vector.h"
#include "..\LibrarySource\ModelResource.h"
class AnimationBlend : public Component
{
private:
	using Sampler = Source::ModelResource::AnimationTake;

	std::vector<Sampler*> m_samplersTake;
	std::queue<std::pair<int , Sampler*>> m_samplers;
	std::vector<std::vector<std::string>> m_boneNames;

public:
	int m_blendLayerCount;
	float m_blendRatio;
	std::vector<std::vector<FLOAT4X4>> m_blendLocals;
	std::vector<std::vector<FLOAT4X4>> m_blendLocalsToParents;
	FLOAT4X4 m_blendActor;
public:
	AnimationBlend() = default;
	~AnimationBlend() = default;

	bool Init() override;

	void Update(float& elapsedTime)override;

	void Render()override {};

	void UpdateRuntimeParameters();
};
