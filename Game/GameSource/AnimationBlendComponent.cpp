#include <assert.h>
#include <functional>
#include "AnimationBlendComponent.h"
#include "ActorModelComponent.h"
#include "..\LibrarySource\Function.h"
#include "..\LibrarySource\SamplingJob.h"
#include "..\LibrarySource\BlendingJob.h"

bool AnimationBlend::Init()
{
	auto model = _entity->FindComponent<ActorModel>().GetActorModel();

	for (int i = 0; i < static_cast<int>(model->_resource->_animationTakes.size()); ++i)
	{
		Sampler& sampler = model->_resource->_animationTakes.at(i);

		m_samplersTake.push_back(&sampler);
	}

	int size = static_cast<int>(model->_resource->_meshes.size());
	m_boneNames.clear();
	m_boneNames.resize(size);
	m_blendLocals.resize(size);
	m_blendLocalsToParents.resize(size);
	for (int i = 0; i < size; ++i)
	{
		auto bufferSize = model->_resource->_meshes.at(i).animations[0].skeletalTransform[0].bones.size();
		m_blendLocals[i].resize(bufferSize);
		m_blendLocalsToParents[i].resize(bufferSize);

		for (auto& bone : model->_resource->_meshes.at(i).offsetTransforms)
		{
			m_boneNames.at(i).emplace_back(bone.name);
		}

		for (auto& animation : model->_resource->_meshes.at(i).animations)
		{
			animation.locals.resize(bufferSize);
			animation.localsToParent.resize(bufferSize);
		}
	}

	m_samplers.push(std::make_pair(0,m_samplersTake.at(0)));
	m_samplers.push(std::make_pair(1, m_samplersTake.at(1)));


	m_blendRatio = 0.5f;
	m_blendLayerCount = 2;
	return true;
}


void AnimationBlend::Update(float& elapsedTime)
{
	if (m_samplers.size() == m_blendLayerCount)
		UpdateRuntimeParameters();


	std::pair<int, Sampler*> samplers[2] = { m_samplers.front(),m_samplers.back() };

	auto model = _entity->FindComponent<ActorModel>().GetActorModel();

	for (int i = 0; i < m_blendLayerCount; ++i)
	{
		Sampler& sampler = *samplers[i].second;
		sampler.animtion.UpdateRatio(sampler.duration,elapsedTime);

		if (sampler.weight <= 0.0f)
			continue;

		for (auto& mesh : model->_resource->_meshes)
		{
			Source::Motion::SamplingJob samplingJob;
			samplingJob.animation = &mesh.animations[samplers[i].first];
			samplingJob.ratio = sampler.animtion.GetTimeRatio();
			samplingJob.duration = sampler.duration;
			samplingJob.output = &mesh.animations[samplers[i].first].locals;
			samplingJob.outputToParents = &mesh.animations[samplers[i].first].localsToParent;
			samplingJob.outputActor = &m_blendActor;
			if (!samplingJob.Run())
				assert(!"samplingJob Error");
		}

	}

	std::vector<Source::Motion::BlendingJob::Layer> layers;
	layers.resize(m_blendLayerCount);
	for (int i = 0; i < m_blendLayerCount; ++i)
	{
		for (auto& mesh : model->_resource->_meshes)
		{
			layers[i].transform.push_back(&mesh.animations[samplers[i].first].locals);
			layers[i].transformToParent.push_back(&mesh.animations[samplers[i].first].localsToParent);
		}
		layers[i].weight = samplers[i].second->weight;
	}
	Source::Motion::BlendingJob blendingJob;
	blendingJob.layer = &layers;
	blendingJob.node = &model->_resource->_rootNode;
	blendingJob.boneNames = &m_boneNames;
	blendingJob.output = &m_blendLocals;
	blendingJob.outputToParent = &m_blendLocalsToParents;

	if (!blendingJob.Run())
		assert(!"blending Error");
}

void AnimationBlend::UpdateRuntimeParameters()
{

	std::pair<int, Sampler*> samplers[2] = { m_samplers.front(),m_samplers.back() };
	
	// Computes weight parameters for all samplers.
	const float kNumIntervals = m_blendLayerCount - 1;
	const float kInterval = 1.f / kNumIntervals;
	for (int i = 0; i < m_blendLayerCount; ++i)
	{
		const float med = i * kInterval;
		const float x = m_blendRatio - med;
		const float y = ((x < 0.f ? x : -x) + kInterval) * kNumIntervals;
		samplers[i].second->weight = Source::Math::Max(0.f, y);
	}

	const int relevant_sampler =
		static_cast<int>((m_blendRatio - 1e-3f) * (m_blendLayerCount - 1));
	assert(relevant_sampler + 1 < m_blendLayerCount);
	Sampler& sampler_l = *samplers[relevant_sampler].second;
	Sampler& sampler_r = *samplers[relevant_sampler + 1].second;

	const float loop_duration =
		sampler_l.duration * sampler_l.weight +
		sampler_r.duration * sampler_r.weight;

	const float inv_loop_duration = 1.f / loop_duration;

	for (int i = 0; i < m_blendLayerCount; ++i) {
		Sampler& sampler = *samplers[i].second;
		const float speed = sampler.duration * inv_loop_duration;
		sampler.animtion.SetPlayBackSpeed(speed);
	}
}
