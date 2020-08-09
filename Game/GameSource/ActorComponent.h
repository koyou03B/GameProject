#pragma once
#include ".\LibrarySource\EntityComponentSystem.h"
#include ".\LibrarySource\Vector.h"
class Actor : public Component
{
private:
	VECTOR3F m_velocity;
	float m_rotationalSpeed;
public:
	Actor() = default;
	~Actor() {};

	bool Init() override;

	void Update(float& elapsedTime) override;

	void Render() override {};

	inline VECTOR3F& GetVelocity() { return m_velocity; }
	inline float& GetRotationalSpeed() { return m_rotationalSpeed; }
};