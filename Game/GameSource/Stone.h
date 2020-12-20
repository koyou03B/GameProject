#pragma once
#include <vector>
#include <memory>
#include "Market.h"
#include "CharacterParameter.h"
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\StaticMesh.h"
#include ".\LibrarySource\InstanceData.h"

struct StoneParam
{
	VECTOR3F	speed = {};
	VECTOR3F	velocity = {};
	bool		isFlying = false;
	float		upPower= 0.0f;

	inline void Clear()
	{
		speed = {};
		velocity = {};
		upPower = 0.0f;
	}

	void ReadBinary()
	{
		if (PathFileExistsA((std::string("../Asset/Binary/Enemy/Stone/StoneParameter") + ".bin").c_str()))
		{
			std::ifstream ifs;
			ifs.open((std::string("../Asset/Binary/Enemy/Stone/StoneParameter") + ".bin").c_str(), std::ios::binary);
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}

	void SaveBinary()
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/Enemy/Stone/StoneParameter") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 1)
		{
			archive
			(
				speed,
				upPower
			);
		}
	};
};
CEREAL_CLASS_VERSION(StoneParam, 1);

using  InstanceData = Source::InstanceData::InstanceData;
class Stone : public ObjectProduct
{
public:
	Stone() = default;
	~Stone() = default;

	void Init()override;
	void Update(float& elapsedTime)override;
	void Render(ID3D11DeviceContext* immediateContext) override;
	
	void Clear();
	void Reset(std::pair<InstanceData, StoneParam>& stoneData);
	void Release();
	void PrepareForStone(const VECTOR3F& position, const VECTOR3F& angle, const VECTOR3F& velocity);

	void SetStoneParameter();
	void ImGui(ID3D11Device* device);

	const VECTOR3F& GetOffsetX() { return m_offsetX; }
	const VECTOR3F& GetOffsetZ() { return m_offsetZ; }
	std::vector<std::pair<InstanceData, StoneParam>>& GetStoneParam()		{ return m_stoneData; }

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 2)
		{
			archive
			(
				m_stoneUpPower,
				m_stoneScale,
				m_offsetX,
				m_offsetZ
			);
		}
	};
private:
	float		m_stoneUpPower = 0.0f;
	VECTOR3F	m_stoneScale = {0.5f,0.5f,0.5f };
	VECTOR3F	m_offsetX = {};
	VECTOR3F	m_offsetZ = {};
	CharacterParameter::Collision						m_collision;
	std::vector<InstanceData>							m_renderData;
	std::shared_ptr<Source::StaticMesh::StaticMesh>		m_model;
	std::vector<std::pair<InstanceData, StoneParam>>	m_stoneData;

};
