#pragma once
#include <memory>
#include <vector>
#include <d3d11.h>
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\InstanceData.h"
#include ".\LibrarySource\GeometricPrimitve.h"

class DebugObject
{
public:
	DebugObject() : m_isWireFrame(0) {};
	~DebugObject() = default;

	void Render(ID3D11DeviceContext* immediateContext,const VECTOR4F& scrollValue,bool isWireFrame = true);

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

	bool IsGeomety()
	{
		if (m_geomtry)
			return true;

		return false;
	}

	inline  Source::InstanceData::InstanceData& GetInstanceData(int number)
	{
		if (static_cast<size_t>(number) >= m_instanceData.size())
			assert("This number is out of range of the m_data");

		return m_instanceData.at(number);
	}
	
	inline std::vector<Source::InstanceData::InstanceData>& GetInstance()
	{
		return m_instanceData;
	}

private:
	std::unique_ptr<Source::GeometricPrimitive::GeometricPrimitive> m_geomtry;
	std::vector<Source::InstanceData::InstanceData> m_instanceData;
	bool m_isWireFrame=true;
};