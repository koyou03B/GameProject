#include "Scope.h"
#include ".\LibrarySource\Input.h"
#include ".\LibrarySource\Function.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif

void Scope::Update(VECTOR3F target)
{
	//******************
	// System：LockON
	//******************
	m_target = target;
	
	VECTOR3F screenPosition = {};
	target.y += m_yAxisValue;
	Source::Math::WorldToScreen(&screenPosition, target);
	m_texturePosition.x = screenPosition.x;
	m_texturePosition.y = screenPosition.y;
	m_screenPosition = screenPosition;
}

void Scope::ImGui(ID3D11Device* device)
{
#if _DEBUG
//**************************************
// Aim
//**************************************
	if (ImGui::CollapsingHeader("Aim"))
	{
		ImGui::SliderFloat("YValue", &m_yAxisValue, 0.0f, Framework::GetInstance().SCREEN_HEIGHT);

		static float scale = m_textureScale.x;
		ImGui::SliderFloat("Scale", &scale, 0.0f, 512.0f);
		m_textureScale.x = m_textureScale.y = scale;

		ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。

		ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&m_textureColor), flag);

		if (ImGui::Button("Save"))
			SaveBinary();
	}
#endif
}

VECTOR3F& Scope::GetWorldScreenPosition()
{
	Source::Math::ScreenToWorld(&m_worldPosition, m_screenPosition);
	return m_worldPosition;
}
