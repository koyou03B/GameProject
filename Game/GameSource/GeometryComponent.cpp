#include "GeometryComponent.h"
#include ".\LibrarySource\Framework.h"
#include ".\LibrarySource\Camera.h"

void Geometry::Render()
{
	if (!m_instanceData.empty())
	{
		auto camera = Source::CameraControlle::CameraManager::GetInstance();
		m_geomtry->Begin(Framework::GetContext(), isWireFrame);
		m_geomtry->Render(Framework::GetContext(), camera->GetView(), camera->GetProjection(),
			m_instanceData, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
		m_geomtry->End(Framework::GetContext());
	}
}
