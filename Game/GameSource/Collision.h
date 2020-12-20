#pragma once
#include <DirectXMath.h>
#include "..\LibrarySource\Vector.h"

class Collision
{
public:
	Collision() = default;
	virtual ~Collision() = default;
	
	struct AABB
	{
		float left;
		float right;
		float top;
		float down;
	};

	struct AABB3D
	{
		DirectX::XMFLOAT3 max;
		DirectX::XMFLOAT3 min;

	};
	VECTOR3F JudgePointAndAABB(const VECTOR3F& myself, const AABB& target);
	bool	 JudgePointAndAABB(const VECTOR2F& myself, const AABB& target);

	bool JudgeAABBAndAABB(const AABB& myself, const AABB& target);
	bool JudgeAABB3DAndAABB3D(const AABB3D& myself, const AABB3D& target);

	struct Sphere
	{
		VECTOR3F position;
		float scale;
		float radius;
	};
	bool JudgeSphereAndSphere(const Sphere& myself, const Sphere& target);

	struct Capsule
	{
		VECTOR3F startPos;
		VECTOR3F endPos;
		float scale;
		float radius;
	};
	bool JudgeCapsuleAndSphere(const Capsule& myself, const Sphere& target);

	struct Circle
	{
		VECTOR2F position = {};
		float scale = .0f;
		float radius = 0.0f;
	};
	bool JudgeCircleAndCircle(const Circle& myself, const Circle& target);
	bool JudgeCircleAndpoint(const Circle& mySelf, const VECTOR2F& target);

	struct Cylinder
	{
		VECTOR3F startPos;
		VECTOR3F endPos;
		float scale;
		float radius;
	};
	bool JudgeCylinderAndCylinder(const Cylinder& myself, const Cylinder& target);

};
