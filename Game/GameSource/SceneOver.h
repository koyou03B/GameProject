#pragma once

#include <memory>
#include "Scene.h"

#include ".\LibrarySource\BillBoard.h"
#include ".\LibrarySource\GeometricPrimitve.h"
#include ".\LibrarySource\Camera.h"
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\ConstantBuffer.h"
#include ".\LibrarySource\Constants.h"
#include ".\LibrarySource\EntityComponentSystem.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\SkinnedMesh.h"
#include ".\LibrarySource\StaticMesh.h"

class Over : public Scene
{
private:
	std::unique_ptr<Source::BillBoard::BillBoard> m_board;
	std::unique_ptr<Source::ConstantBuffer::ConstantBuffer<Source::Constants::SceneConstants>> m_sceneConstantBuffer;
	std::shared_ptr<Source::SkinnedMesh::SkinnedMesh> m_actModel;
	std::shared_ptr<Source::StaticMesh::StaticMesh> m_staticModel;

	VECTOR3F scale = { 1.0f, 1.0f, 1.0f };
	VECTOR3F angle = { 0.0f, 180.0f * 0.01745f, 0.0f };
	VECTOR3F translate = { 15.0f, 1.0f, .0f };
	VECTOR4F direction = { sinf(angle.y * 0.01745f), 0.0f, cosf(angle.y * 0.01745f),1.0f };
	float focalLength = 5.0f;
public:
	Over() = default;
	virtual ~Over() = default;

	bool Initialize(ID3D11Device* device)override;
	void Update(float& elapsedTime)override;
	void Render(ID3D11DeviceContext* immediateContext, float elapsedTime)override;
	void ImGui()override;
	void Uninitialize()override;
};
