#include <functional>
#include "PartialBlendAnimation.h"
#include ".\LibrarySource\SamplingJob.h"
#include ".\LibrarySource\BlendingJob.h"

bool PartialBlendAnimation::Init(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model)
{
	int meshCount = static_cast<int>(model->_resource->_meshes.size());
	_blendLocals.resize(meshCount);
	_blendLocalsToParents.resize(meshCount);
	m_partialBones.resize(meshCount);
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

		for (auto& boneName : mesh.offsetTransforms)
		{
			m_boneNames[i].push_back(boneName.name);
		}
	}

	if (m_samplers.size() <= 0)
	{
		auto& lowerBodySampler = model->_resource->_animationTakes[2];
		auto& upBodySampler = model->_resource->_animationTakes[8];

		lowerBodySampler.weight = 1.0f;
		upBodySampler.weight = 1.0f;

		m_samplers.push_back(std::make_pair(2, &lowerBodySampler));
		m_samplers.push_back(std::make_pair(8, &upBodySampler));
	}

	{
		int samplerCount = static_cast<int>(m_samplers.size());
		m_partialBones.resize(samplerCount);

		_upperBodyRoot = 3;

		auto& lowerBodyBone = m_partialBones[0];
		auto& upperBodyBone = m_partialBones[1];

		lowerBodyBone.weight = 0.0f;
		lowerBodyBone.halfBodyBone.resize(meshCount);

		upperBodyBone.weight = 1.0f;
		upperBodyBone.halfBodyBone.resize(meshCount);

		SetupNodesForVector(model->_resource->_rootNode);
		SetupHalfBody(model);
	}

	m_animationFrame.resize(5);
	for (int i = 0; i < 5; ++i)
	{
		m_animationFrame.at(i) = 0;
	}
	return true;
}

void PartialBlendAnimation::Update(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model, float& elapsedTime)
{
	std::pair<int, Sampler*> samplers[2] = { m_samplers.front(),m_samplers.back() };


	const int kNumLayers = static_cast<int>(m_samplers.size());
	for (int i = 0; i < kNumLayers; ++i)
	{
		Sampler& sampler = *samplers[i].second;
		sampler.animtion.UpdateRatio(sampler.duration, elapsedTime);

		if (sampler.weight <= 0.0f)
			continue;

		int numSampler = samplers[i].first;
		for (auto& mesh : model->_resource->_meshes)
		{
			Source::Blend::SamplingJob sampligJob;
			sampligJob._animation = &mesh.animations[numSampler];
			sampligJob._ratio = sampler.animtion.GetTimeRatio();
			sampligJob._output = &mesh.animations[numSampler].locals;
			sampligJob._outputToParents = &mesh.animations[numSampler].localsToParent;
			sampligJob._duration = sampler.duration;
			sampligJob._animationFrame = &m_animationFrame[i];
			if (!sampligJob.Run())
				assert(!"samplingJob Error");
		}

	}
	std::vector<Source::Blend::PartialAnimationBlendingJob::Layer> layers;
	layers.resize(kNumLayers);

	for (int i = 0; i < kNumLayers; ++i)
	{
		int numSample = samplers[i].first;
		Sampler& sampler = *samplers[i].second;

		for (auto& mesh : model->_resource->_meshes)
		{
			layers[i].transform.push_back(&mesh.animations[numSample].locals);
			layers[i].transformToParent.push_back(&mesh.animations[numSample].localsToParent);
		}
		layers[i].weight = m_partialBones[i].weight;


		for (auto& bone : m_partialBones[i].halfBodyBone)
		{
			layers[i].partialBlendWeight.push_back(bone);
		}

	}

	Source::Blend::PartialAnimationBlendingJob blendingJob;
	blendingJob._layer = &layers;
	blendingJob._node = &model->_resource->_rootNode;
	blendingJob._boneNames = &m_boneNames;
	blendingJob._output = &_blendLocals;
	blendingJob._outputToParent = &_blendLocalsToParents;

	if (!blendingJob.Run())
		assert(!"blendingJob Error");
}

void PartialBlendAnimation::SetupNodesForVector(Source::ModelResource::HierarchyNode& node)
{
	std::function<void(Source::ModelResource::HierarchyNode&, std::vector<Source::ModelResource::HierarchyNode>& )> traverseHierarchy =
		[&traverseHierarchy](Source::ModelResource::HierarchyNode& parent, std::vector<Source::ModelResource::HierarchyNode>& nodes)
	{
		nodes.push_back(parent);

		if (!parent.chirdlen.empty())
		{
			auto numberOfChildren = parent.chirdlen.size();
			for (int i = 0; i < numberOfChildren; i++)
			{
				traverseHierarchy(parent.chirdlen[i], nodes);
			}
		}
	};

	traverseHierarchy(node,m_nodes);
}

void PartialBlendAnimation::SetupHalfBody(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model)
{
	HalfBodyLayer& lowerBodyBone = m_partialBones[0];
	HalfBodyLayer& upperBodyBone = m_partialBones[1];

	auto meshCount = model->_resource->_meshes.size();

	for (int i = 0; i < meshCount; ++i)
	{
		if (lowerBodyBone.halfBodyBone[i].size() != 0)
		{
			lowerBodyBone.halfBodyBone[i].clear();
			upperBodyBone.halfBodyBone[i].clear();
		}

		for (auto v : model->_resource->_meshes[i].offsetTransforms)
		{
			lowerBodyBone.halfBodyBone[i].push_back(1.0f);
			upperBodyBone.halfBodyBone[i].push_back(0.0f);
		}

		SearchSelectBone(lowerBodyBone, i);
		SearchSelectBone(upperBodyBone, i);

	}
}


void PartialBlendAnimation::SearchSelectBone(HalfBodyLayer& bodyBone, int currentMesh)
{
	auto boneCount = m_nodes.size();

	for (int i = _upperBodyRoot < 0 ? 0 : _upperBodyRoot, process = i < boneCount;
		process;
		++i, process = i < boneCount && SearchNodeIndex(m_nodes[i].parentName) >= _upperBodyRoot)
	{
		int currentBone = SearchBoneNameIndex(m_nodes[i].name,currentMesh);
		if (currentBone == -1)
			continue;

		bodyBone.halfBodyBone[currentMesh].at(currentBone) = bodyBone.weight;
	}

}

