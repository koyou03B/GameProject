#pragma once
#include <memory>
#include <vector>
#include ".\LibrarySource\EntityComponentSystem.h"
#include ".\LibrarySource\StaticMesh.h"
#include ".\LibrarySource\InstanceData.h"

class StaticModel : public Component
{
private:
	std::shared_ptr<Source::StaticMesh::StaticMesh> m_staticModel;
	std::vector<Source::InstanceData::InstanceData> m_instanceData;

public:
	StaticModel() = default;
	~StaticModel() {};

	bool Init() override { return true; };
	void Update(float& elapsedTime) override {};
	void Render() override;

	int RayPick(
		const VECTOR3F& startPosition,
		const VECTOR3F& endPosition,
		VECTOR3F* outPosition,
		VECTOR3F* outNormal
	);

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


	inline std::shared_ptr<Source::StaticMesh::StaticMesh>& GetStaticModel() { return m_staticModel; }

	inline const Source::InstanceData::InstanceData& GetInstanceData(int number)
	{
		if (static_cast<size_t>(number) >= m_instanceData.size())
			assert("This number is out of range of the m_data");

		return m_instanceData.at(number);
	}

	inline std::vector<Source::InstanceData::InstanceData>& GetInstanceData() { return m_instanceData; }

	inline void SetStaticModel(std::shared_ptr<Source::StaticMesh::StaticMesh> staticModel)
	{
		m_staticModel = staticModel;
	}

};