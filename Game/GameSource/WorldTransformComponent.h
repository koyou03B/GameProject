#pragma once
#include ".\LibrarySource\EntityComponentSystem.h"
#include ".\LibrarySource\Vector.h"

class WorldTransform : public Component
{
private:
	VECTOR3F m_translate;
	VECTOR3F m_angle;
	VECTOR3F m_scale;
	FLOAT4X4 m_world;

public:

	WorldTransform() = default;
	~WorldTransform() {};

	bool Init() override;

	void Update(float& elapsedTime) override {};

	void Render() override {};

	void WorldUpdate()
	{
		DirectX::XMMATRIX S, R, T;
		S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
		R = DirectX::XMMatrixRotationRollPitchYaw(m_angle.x, m_angle.y, m_angle.z);
		T = DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z);
		DirectX::XMStoreFloat4x4(&m_world, S * R * T);
	}

	inline const FLOAT4X4 GetWorld() { return m_world; }
	inline const VECTOR3F GetTranslate() { return m_translate; }
	inline const VECTOR3F GetAngle() { return m_angle; }
	inline const VECTOR3F GetScale() { return m_scale; }

	inline void SetTranslate(VECTOR3F& transrate) { m_translate = transrate; }
	inline void SetAngle(VECTOR3F& angle) { m_angle = angle; }
	inline void SetScale(VECTOR3F& scale) { m_scale = scale; }
};