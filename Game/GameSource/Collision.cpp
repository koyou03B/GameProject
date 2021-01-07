#include "Collision.h"

VECTOR3F Collision::JudgePointAndAABB(const VECTOR3F& mySelf, const AABB& target)
{
	VECTOR3F returnValue = { mySelf.x , mySelf.y,mySelf.z};
	if (mySelf.x <= target.left)
		returnValue.x = target.left;
	if(mySelf.x >= target.right)
		returnValue.x = target.right;
	if (mySelf.z <= target.down)
		returnValue.z = target.down;
	if(mySelf.z >= target.top)
		returnValue.z = target.top;

	return returnValue;
}

bool Collision::JudgePointAndAABB(const VECTOR2F& mySelf, const AABB& target)
{
	if (mySelf.x >= target.left && mySelf.x <= target.right &&
		mySelf.y >= target.down && mySelf.y <= target.top)
		return true;

	return false;
}

bool Collision::JudgeAABBAndAABB(const AABB& mySelf, const AABB& target)
{
	if (mySelf.left < target.left)		return false;
	if (mySelf.top > target.top)		return false;
	if (mySelf.right > target.right)	return false;
	if (mySelf.down > target.down)		return false;

	return true;
}

bool Collision::JudgeAABB3DAndAABB3D(const AABB3D& mySelf, const AABB3D& target)
{

	if (mySelf.max.x < target.min.x)return false;
	if (mySelf.min.x > target.max.x)return false;
	if (mySelf.max.y < target.min.y)return false;
	if (mySelf.min.y > target.max.y)return false;
	if (mySelf.max.z < target.min.z)return false;
	if (mySelf.min.z > target.max.z)return false;

	return true;
}

bool Collision::JudgeSphereAndSphere(const Sphere& mySelf, const Sphere& target)
{
	const float radius = (mySelf.scale * mySelf.radius + target.scale * target.radius) *
		(mySelf.scale * mySelf.radius + target.scale * target.radius);

	VECTOR3F distance;
	distance = mySelf.position - target.position;

	const float length = (distance.x * distance.x) + (distance.y * distance.y) + (distance.z * distance.z);

	if (length < radius)
		return true;

	return false;
}


bool Collision::JudgeCapsuleAndSphere(const Capsule& mySelf, const Sphere& target)
{
	VECTOR3F vector = mySelf.endPos - mySelf.startPos;//Capsuleのスタートポジションとエンドポジション間のベクトル
	float scalar = ToDistVec3(vector);//上のベクトル大きさ

	vector.x /= scalar;
	vector.y /= scalar;
	vector.z /= scalar;


	VECTOR3F targetVector = mySelf.startPos - target.position;//Capsuleのスタートポジションとplayerの攻撃判定ポジション間のベクトル
	float targetScalar = ToDistVec3(targetVector);//上のベクトルの大きさ

	targetVector.x /= targetScalar;
	targetVector.y /= targetScalar;
	targetVector.z /= targetScalar;

	float dot = DotVec3(vector, targetVector);//上二つのベクトルで内積

	//----------------------------------------------------------------
	//　　内積の範囲
	//	　http://nomoreretake.net/2015/12/01/what_is_dotprocudt_math/
	//----------------------------------------------------------------

	if (dot <= 0)//内積が0以下なら球の当たり判定
	{
		Sphere sphere;
		sphere.position = mySelf.startPos;
		sphere.radius = mySelf.radius;
		sphere.scale = mySelf.scale;

		if (JudgeSphereAndSphere(sphere, target))
			return true;
		else
		{
			sphere.position = mySelf.endPos;
			if (JudgeSphereAndSphere(sphere, target))
				return true;
		}
	}
	else
	{
		float pro = (dot / scalar * scalar);//射影ベクトルの式のベクトルを掛ける前が共通してるので
		VECTOR3F projection;
		projection.x = pro * vector.x;//射影ベクトル
		projection.y = pro * vector.y;//射影ベクトル
		projection.z = pro * vector.z;//射影ベクトル

		VECTOR3F position;
		position = projection + mySelf.startPos;//カプセルのスタートポジションから射影ベクトル分移動させたポジション

		VECTOR3F vector;
		vector = target.position - position;//プレイヤーの攻撃判定ポジションと射影ポジション間のベクトル

		scalar = ToDistVec3(vector);//上のベクトルの大きさ

		float r = mySelf.radius * mySelf.scale + target.radius * target.scale;

		if (scalar <= r)
			return true;
	}

	return false;
}


bool Collision::JudgeCircleAndCircle(const Circle& mySelf, const Circle& target)
{
	const float radius = (mySelf.scale * mySelf.radius + target.scale * target.radius) *
		(mySelf.scale * mySelf.radius + target.scale * target.radius);

	VECTOR2F distance;
	distance = mySelf.position - target.position;

	const float length = (distance.x * distance.x) + (distance.y * distance.y);

	if (length < radius)
		return true;

	return false;
}

bool Collision::JudgeCircleAndpoint(const Circle& mySelf, const VECTOR2F& target)
{
	const float radius = mySelf.scale * mySelf.radius;

	float distance;
	distance = ToDistVec2(target);

	if (distance > radius)
		return true;

	return false;
}

bool Collision::JudgeCylinderAndCylinder(const Cylinder& mySelf, const Cylinder& target)
{
	Circle mine, yours;
	mine.position = VECTOR2F(mySelf.bottom.x, mySelf.bottom.z);
	mine.radius = mySelf.radius;
	mine.scale = mySelf.scale;

	yours.position = VECTOR2F(target.bottom.x, target.bottom.z);
	yours.radius = target.radius;
	yours.scale = target.scale;

	if (JudgeCircleAndCircle(mine, yours))
	{
		if (mySelf.bottom.y <= target.bottom.y && target.bottom.y <= mySelf.top.y)
			return true;
		if (mySelf.bottom.y <= target.top.y && target.top.y <= mySelf.top.y)
			return true;
		if (target.bottom.y <= mySelf.bottom.y && mySelf.bottom.y <= target.top.y)
			return true;
	}

	return false;
}
