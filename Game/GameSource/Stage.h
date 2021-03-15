#pragma once
#include <d3d11.h>
#include <memory>
#include <vector>
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\StaticMesh.h"
#include ".\LibrarySource\InstanceData.h"
#include ".\LibrarySource\ModelData.h"
#include "Collision.h"

#undef max
#undef min
#include<cereal/cereal.hpp>
#include<cereal/archives/binary.hpp>
#include<cereal/types/vector.hpp>

class Stage
{
public:
	Stage() = default;
	~Stage() = default;

	void Init();

	void Update(float& elapsedTime);

	bool FanceBreak(float& elapsedTime);

	void Render(ID3D11DeviceContext* immediateContext);

	void Release()
	{
		if (m_model)
		{
			if (m_model[0].unique())
			{
				m_model[0].reset();
			}
			if (m_model[1].unique())
			{
				m_model[1].reset();
			}
		}
	}

	int RayPick(
		const VECTOR3F& startPosition,
		const VECTOR3F& endPosition,
		VECTOR3F* outPosition,
		VECTOR3F* outNormal);

	int MoveCheck(
		const VECTOR3F& startPosition,
		const VECTOR3F& endPosition,
		VECTOR3F* outPosition);


	void ImGui();

	void AddInstanceData(const VECTOR3F& position, const VECTOR3F& angle, const VECTOR3F& scale, const VECTOR4F& color);

	inline std::vector<Source::InstanceData::InstanceData>& GetInstanceData() { return m_instanceData; }

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 0)
		{
			archive
			(
				m_instanceData
			);
		}
	};

private:
	std::shared_ptr<Source::StaticMesh::StaticMesh> m_model[2];
	std::vector<Source::InstanceData::InstanceData> m_instanceData;
	std::vector<std::string> m_meshNames;
	Collision::Circle m_circle;
	VECTOR3F m_position;
	VECTOR3F m_angle;
	VECTOR3F m_scale;
	FLOAT4X4 m_world;
	int m_state;
	float m_timer;
};