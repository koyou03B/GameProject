#pragma once
#include <d3d11.h>
#include <memory>
#include <vector>
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\StaticMesh.h"
#include ".\LibrarySource\InstanceData.h"

class Stage
{
public:
	Stage() = default;
	~Stage() = default;

	void Init();

	void Update(float& elapsedTime) {};

	void Render(ID3D11DeviceContext* immediateContext);

	void ImGui();

	void AddInstanceData(const VECTOR3F& position, const VECTOR3F& angle, const VECTOR3F& scale, const VECTOR4F& color)
	{
		Source::InstanceData::InstanceData data;
		data.position = position;
		data.angle = angle;
		data.scale = scale;
		data.color = color;
		data.CreateWorld();

		m_instanceData.emplace_back(data);
	}

	inline std::shared_ptr<Source::StaticMesh::StaticMesh>& GetStaticModel() { return m_model; }

	inline std::vector<Source::InstanceData::InstanceData>& GetInstanceData() { return m_instanceData; }


private:
	std::shared_ptr<Source::StaticMesh::StaticMesh> m_model;
	std::vector<Source::InstanceData::InstanceData> m_instanceData;

};