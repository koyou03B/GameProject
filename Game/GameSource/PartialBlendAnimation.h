#pragma once
#include <Shlwapi.h>
#include "AnimationBlend.h"
#include "LibrarySource\HalfBodyLayer.h"

class PartialBlendAnimation : public AnimationBlend
{
	using HalfBodyLayer = Source::Blend::HalfBodyLayer;
public:
	int _upperBodyRoot;

public:
	bool Init(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model) override;

	void Update(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model, float& elapsedTime) override;

	void SetupHalfBody(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& model);

	void SetupNodesForVector(Source::ModelResource::HierarchyNode& node);

	void SearchSelectBone(HalfBodyLayer& bodyBone, int currentMesh);

	std::vector<HalfBodyLayer>& GetPartialBoens() { return m_partialBones; };

	std::vector<Source::ModelResource::HierarchyNode>& GetNodes() { return m_nodes; }

	int SearchBoneNameIndex(std::string searchBoneName,int meshNo)
	{
		auto boneNameCount = m_boneNames.at(meshNo).size();
		for (int i = 0; i < boneNameCount; ++i)
		{
			if (m_boneNames.at(meshNo).at(i) == searchBoneName)
				return i;
		}

		return -1;
	}

	int SearchNodeIndex(std::string searchName)
	{
		auto nodesCount = m_nodes.size();
		for (int i = 0; i < nodesCount; ++i)
		{
			if (m_nodes[i].name == searchName)
				return i;
		}
		return -1;
	}

private:
	std::vector<HalfBodyLayer> m_partialBones;
	std::vector<Source::ModelResource::HierarchyNode> m_nodes;

};