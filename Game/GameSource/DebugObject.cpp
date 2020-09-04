#include "DebugObject.h"
#include ".\LibrarySource\Camera.h"

void DebugObject::Render(ID3D11DeviceContext* immediateContext, const VECTOR4F& scrollValue,bool isScroll)
{
	if (!m_instanceData.empty())
	{
		auto camera = Source::CameraControlle::CameraManager::GetInstance();
		m_geomtry->Begin(immediateContext, isScroll, m_isWireFrame);
		m_geomtry->Render(immediateContext, camera->GetView(), camera->GetProjection(),
			m_instanceData, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f), scrollValue);
		m_geomtry->End(immediateContext);
	}
}
