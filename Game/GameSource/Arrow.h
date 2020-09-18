#pragma once
#include <vector>
#include <memory>
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\StaticMesh.h"
#include ".\LibrarySource\InstanceData.h"

class Arrow
{
public:
	void Init();

	void Update(float& elapsedTime) ;

	void Render(ID3D11DeviceContext* immediateContext) ;

	void ImGui(ID3D11Device* device) ;
private:
	std::unique_ptr<Source::StaticMesh::StaticMesh> m_model;

	std::vector<Source::InstanceData::InstanceData> m_worldPrams;
	
	VECTOR3F m_speed;
	bool m_exit;
	int m_timer;

};

class ArrowAdominist
{
public:
	void Init() ;

	void Update(float& elapsedTime) ;

	void Render(ID3D11DeviceContext* immediateContext) ;

	void ImGui(ID3D11Device* device) ;

	void Release();

	inline ArrowAdominist& GetInstace()
	{
		static 	ArrowAdominist arrowAdomin;
		return arrowAdomin;
	}

private:
	Arrow m_arrow;
};