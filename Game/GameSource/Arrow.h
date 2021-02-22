#pragma once
#include <vector>
#include <memory>
#include "Market.h"
#include "DebugObject.h"
#include "CharacterParameter.h"
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\StaticMesh.h"
#include ".\LibrarySource\InstanceData.h"

struct ArrowParam
{
	VECTOR3F	speed = {};
	VECTOR3F	velocity = {};
	bool		isFlying = false;
	float		attackPoint = 0.0f;
	float		fallTime = 0.0f;

	inline void Clear()
	{
		speed = {};
		velocity = {};
		isFlying = false;
		attackPoint = 0.0f;
		fallTime = 0.0f;
	}

	void ReadBinary()
	{
		if (PathFileExistsA((std::string("../Asset/Binary/Player/Archer/ArrowParam") + ".bin").c_str()))
		{
			std::ifstream ifs;
			ifs.open((std::string("../Asset/Binary/Player/Archer/ArrowParam") + ".bin").c_str(), std::ios::binary);
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}

	void SaveBinary()
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/Player/Archer/ArrowParam") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 0)
		{
			archive
			(
				speed,
				attackPoint
			);
		}
	};
};
CEREAL_CLASS_VERSION(ArrowParam, 1);

using  InstanceData = Source::InstanceData::InstanceData;

class Arrow : public ObjectProduct
{
public:
	Arrow() = default;
	~Arrow() = default;
	Arrow(const Arrow&) = delete;

	void Init()override;
	void Update(float& elapsedTime)override;
	void Render(ID3D11DeviceContext* immediateContext) override;

	void Clear();
	void Reset(std::pair<InstanceData, ArrowParam>& stoneData);
	void Release();
	void PrepareForArrow(const VECTOR3F& position, const VECTOR3F& angle, const VECTOR3F& velocity);

	void SetArrowParameter();
	void ImGui(ID3D11Device* device);

	std::vector<std::pair<InstanceData, ArrowParam>>& GetArrowParam() { return m_arrowData; }

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 0)
		{
			archive
			(
				m_arrowFallPower,
				m_arrowFallRotate,
				m_defineFallTime,
				m_defineResetLine,
				m_defineFallRotate,
				m_offsetZ
			);
		}
	};
private:
	float		m_arrowFallPower = 0.0f;
	float		m_arrowFallRotate = 0.0f;
	float		m_defineFallTime = 0.0f;
	float		m_defineResetLine = 0.0f;
	float		m_defineFallRotate = 0.0f;
	float		m_offsetZ = {};

	VECTOR3F	m_arrowScale = { 0.5f,0.5f,0.5f };

	CharacterParameter::Collision						m_collision;
	std::vector<InstanceData>							m_renderData;
	std::shared_ptr<Source::StaticMesh::StaticMesh>		m_model;
	std::vector<std::pair<InstanceData, ArrowParam>>	m_arrowData;
	DebugObject m_debugObject;
};
