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
public:
	Over() = default;
	virtual ~Over() = default;

	bool Initialize(ID3D11Device * device)override;
	void Update(float& elapsedTime)override;
	void Render(ID3D11DeviceContext * immediateContext, float elapsedTime)override;
	void ImGui()override;
	void Uninitialize()override;
private:
	std::shared_ptr<Source::Sprite::SpriteBatch>		m_sprite;

	Source::Input::Input* m_input;
	bool isTitle = false;
	bool isGame = false;
	bool isEvent = false;


	VECTOR2F position = {};
	VECTOR2F scale = { 640.0f,640.0f };
	VECTOR2F texPos = {};
	VECTOR2F texSize = { 640.0f,640.0f };
	int m_eventState = 0;

};
