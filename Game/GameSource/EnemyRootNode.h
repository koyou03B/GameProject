#pragma once
#include "EnemyBehaviorNode.h"

class EnemyRootNode : public EnemyBehaviorNode
{
public:
	EnemyRootNode() = default;
	~EnemyRootNode() = default;

	uint32_t JudgePriority(const int id) { return minPriority; }

	void LoadOfBinaryFile(std::string nodeName)
	{
		if (PathFileExistsA((std::string("../Asset/Binary/Enemy/BehaviorNode/") + nodeName + ".bin").c_str()))
		{
			std::ifstream ifs;
			ifs.open((std::string("../Asset/Binary/Enemy/BehaviorNode/") + nodeName + ".bin").c_str()
				, std::ios::binary);
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}

	void SaveOfBinaryFile()
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/Enemy/BehaviorNode/") + m_nodeName + ".bin").c_str()
			, std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (m_serialVersion <= version)
		{
			archive
			(
				m_nodeName,
				m_selectRule,
				m_family
			);
		}
		else
		{
			archive
			(
				m_nodeName,
				m_selectRule,
				m_family
			);
		}
	}
};

CEREAL_CLASS_VERSION(EnemyRootNode, 1);
