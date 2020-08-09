#include "WorldTransformComponent.h"

bool WorldTransform::Init()
{
	m_translate = VECTOR3F(0.0f, 0.0f, 0.0f);
	m_angle = VECTOR3F(0.0f * 0.01745f, 180.0f * 0.01745f, 0.0f * 0.01745f);
	m_scale = VECTOR3F(1.0f, 1.0f, 1.0f);

	WorldUpdate();

	return true;
}
