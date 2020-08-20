#include <assert.h>
#include <memory>
#include "AnimationBlend.h"
#include "..\LibrarySource\Function.h"
#include "..\LibrarySource\SamplingJob.h"
#include "..\LibrarySource\BlendingJob.h"

bool AnimationBlend::Init(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model)
{
	int meshCount = static_cast<int>(model->_resource->_meshes.size());
	_blendLocals.resize(meshCount);
	_blendLocalsToParents.resize(meshCount);
	m_boneNames.resize(meshCount);

	for (int i = 0; i < meshCount; ++i)
	{
		auto& mesh = model->_resource->_meshes[i];
		if (mesh.animations.empty())
			continue;

		auto bufferSize = mesh.animations[0].skeletalTransform[0].bones.size();

		_blendLocals[i].resize(bufferSize);
		_blendLocalsToParents[i].resize(bufferSize);

		for (auto& animation : mesh.animations)
		{
			animation.locals.resize(bufferSize);
			animation.localsToParent.resize(bufferSize);
		}

		for (auto& bone : mesh.animations[0].skeletalTransform[0].bones)
		{
			m_boneNames.at(i).emplace_back(bone.name);
		}

	}

	if (m_samplers.size() <= 0)
	{
		auto& anim = model->_resource->_animationTakes[0];
		m_samplers.push_back(std::make_pair(0, &anim));
		//auto& anim2 = model->_resource->_animationTakes[1];
		//m_samplers.push_back(std::make_pair(1, &anim2));
		//auto& anim3 = model->_resource->_animationTakes[2];
		//m_samplers.push_back(std::make_pair(2, &anim3));
	}

	_blendRatio = 0.0f;
	return true;
}

void AnimationBlend::Update(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model, float& elapsedTime)
{
	UpdateRuntimeParameters();
	const int kNumLayers = static_cast<int>(m_samplers.size());
	for (int i = 0; i < kNumLayers; ++i)
	{
		Sampler& sampler = *m_samplers[i].second;
		sampler.animtion.UpdateRatio(sampler.duration, elapsedTime);

		if (sampler.weight <= 0.0f)
			continue;

		int numSampler = m_samplers[i].first;
		for (auto& mesh : model->_resource->_meshes)
		{
			Source::Blend::SamplingJob sampligJob;
			sampligJob._animation = &mesh.animations[numSampler];
			sampligJob._ratio = sampler.animtion.GetTimeRatio();
			sampligJob._output = &mesh.animations[numSampler].locals;
			sampligJob._outputToParents = &mesh.animations[numSampler].localsToParent;
			sampligJob._duration = sampler.duration;

			if (!sampligJob.Run())
				assert(!"samplingJob Error");
		}
	}
	std::vector<Source::Blend::AnimationBlendingJob::Layer> layers;
	layers.resize(kNumLayers);
	for (int i = 0; i < kNumLayers; ++i)
	{
		int numSample = m_samplers[i].first;
		Sampler& sampler = *m_samplers[i].second;

		for (auto& mesh : model->_resource->_meshes)
		{
			layers[i].transform.push_back(&mesh.animations[numSample].locals);
			layers[i].transformToParent.push_back(&mesh.animations[numSample].localsToParent);
		}
		layers[i].weight = sampler.weight;
	}

	Source::Blend::AnimationBlendingJob blendingJob;
	blendingJob._layer = &layers;
	blendingJob._node = &model->_resource->_rootNode;
	blendingJob._boneNames = &m_boneNames;
	blendingJob._output = &_blendLocals;
	blendingJob._outputToParent = &_blendLocalsToParents;

	if (!blendingJob.Run())
		assert(!"blendingJob Error");


}

void AnimationBlend::UpdateRuntimeParameters()

{
	const float kNumIntervals = static_cast<float>(m_samplers.size() - 1);
	const float kInterval = 1.0f / kNumIntervals;

	if (m_samplers.size() == 1)
	{
		m_samplers[0].second->weight = 1.0f;
		m_samplers[0].second->animtion.SetPlayBackSpeed(1.0);
	}
	else
	{
		for (int i = 0; i < static_cast<int>(m_samplers.size()); ++i)
		{
			const float med = i * kInterval;
			const float x = _blendRatio - med;
			const float y = ((x < 0.0f ? x : -x) + kInterval) * kNumIntervals;
			m_samplers[i].second->weight = Source::Math::Max(0.f, y);
		}

		const int relevantSampler =
			static_cast<int>((_blendRatio - 1e-3f) * (static_cast<int>(m_samplers.size()) - 1));
		assert(relevantSampler + 1 < static_cast<int>(m_samplers.size()));

		Sampler& sampler_l = *m_samplers[relevantSampler].second;
		Sampler& sampler_r = *m_samplers[relevantSampler + 1].second;

		const float loopDuration =
			sampler_l.duration * sampler_l.weight +
			sampler_r.duration * sampler_r.weight;

		const float invLoopDuration = 1.f / loopDuration;

		for (int i = 0; i < static_cast<int>(m_samplers.size()); ++i)
		{
			Sampler& sampler = *m_samplers[i].second;
			const float speed = sampler.duration * invLoopDuration;
			sampler.animtion.SetPlayBackSpeed(speed);
		}

	}

}
