#pragma once
#include"Scene.h"
#include "TitleObjectAdominist.h"
#include ".\LibrarySource\Constants.h"
#include ".\LibrarySource\FrameBuffer.h"
#include ".\LibrarySource\Rasterizer.h"
#include ".\LibrarySource\ScreenFilter.h"
#ifdef _DEBUG
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#endif

class Clear : public Scene
{
public:
	Clear() = default;
	virtual ~Clear() = default;

	bool Initialize(ID3D11Device* device) override;
	void Update(float& elapsedTime) override;
	void Render(ID3D11DeviceContext* immediateContext, float elapsedTime) override;
	void ImGui() override;
	void Uninitialize() override;

private:
	std::shared_ptr<Source::Sprite::SpriteBatch>		m_sprite;
	std::unique_ptr<Source::FrameBuffer::FrameBuffer> m_frameBuffer;
	std::unique_ptr<Source::ScreenFilter::ScreenFilter> m_screenFilter;
	std::unique_ptr<Source::ConstantBuffer::ConstantBuffer<Source::Constants::Dissolve>> m_dissolve;

	Source::Input::Input* m_input;
	bool isTitle = false;
	bool isGame = false;
	bool isEvent = false;


	VECTOR2F position = {};
	VECTOR2F scale = {640.0f,640.0f};
	VECTOR2F texPos = {};
	VECTOR2F texSize = { 640.0f,640.0f };
	int m_eventState = 0;
};