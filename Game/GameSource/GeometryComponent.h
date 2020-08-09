#pragma once
#include <memory>
#include <vector>
#include <map>
#include <assert.h>
#include ".\LibrarySource\EntityComponentSystem.h"
#include ".\LibrarySource\InstanceData.h"
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\GeometricPrimitve.h"

class Geometry : public Component
{
private:
	std::unique_ptr<Source::GeometricPrimitive::GeometricPrimitive> m_geomtry;
	std::vector<Source::InstanceData::InstanceData> m_instanceData;
	bool isWireFrame;

public:
	Geometry() = default;
	~Geometry() {};

	bool Init() override { return true; };

	void Update(float& elapsedTime) override {};

	void Render() override;

	template<typename T>
	void AddGeometricPrimitive(std::unique_ptr<T> geometry)
	{
		m_geomtry = std::move(geometry);
	}

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

	inline const Source::InstanceData::InstanceData& GetInstanceData(int number)
	{
		if (static_cast<size_t>(number) >= m_instanceData.size())
			assert("This number is out of range of the m_data");

		return m_instanceData.at(number);
	}

	inline const std::vector<Source::InstanceData::InstanceData>& GetInstance()
	{
		return m_instanceData;
	}
};