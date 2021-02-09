#include "Vector.h"

float ToDistVec2(const VECTOR2F v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y));
};

VECTOR2F NormalizeVec2(const VECTOR2F v)
{
	float dist = ToDistVec2(v);


	return VECTOR2F(v.x / dist,
		v.y / dist);
};;

VECTOR2F ConvertVec2(const float conv, const VECTOR2F v)
{
	const VECTOR2F normalize_vec2 = NormalizeVec2(v);
	return normalize_vec2 * conv;
}

VECTOR2F LerpVec2(VECTOR2F lerpStart, VECTOR2F lerpEnd, float value)
{
	DirectX::XMVECTOR start, end, lerpPosition;
	VECTOR2F outPosition;
	start = DirectX::XMLoadFloat2(&lerpStart);
	end = DirectX::XMLoadFloat2(&lerpEnd);
	lerpPosition = DirectX::XMVectorLerp(start, end, value);
	DirectX::XMStoreFloat2(&outPosition, lerpPosition);
	return outPosition;
}

VECTOR2F SphereLinearVec2(VECTOR2F origin, VECTOR2F start, VECTOR2F end, float t)
{
	VECTOR2F sphereLinerStart = NormalizeVec2(start - origin);
	VECTOR2F sphereLinerEnd = NormalizeVec2(end - origin);

	// 2ベクトル間の角度（鋭角側）
	float angle = acosf(DotVec2(sphereLinerStart, sphereLinerEnd));

	// sinθ
	float SinTh = sinf(angle);

	// 補間係数
	float Ps = sinf(angle * (1.0f - t));
	float Pe = sinf(angle * t);

	VECTOR2F out;
	if (angle == 0.0f)
	{
		out.x = 0.0f;
		out.y = 0.0f;
	}
	else
	{
		out.x = (Ps * sphereLinerStart.x + Pe * sphereLinerEnd.x) / SinTh;
		out.y = (Ps * sphereLinerStart.y + Pe * sphereLinerEnd.y) / SinTh;
	}

	return out;
}

float CrossVec2(const VECTOR2F v1, const VECTOR2F v2)
{	//ベクトルの外積
	return v1.x * v2.y - v1.y * v2.x;
};

float DotVec2(const VECTOR2F v1, const VECTOR2F v2)
{	//ベクトルの内積
	return v1.x * v2.x + v1.y * v2.y;
};

float ToDistVec3(const VECTOR3F v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

VECTOR3F NormalizeVec3(const VECTOR3F v)
{
	float dist = ToDistVec3(v);


	return VECTOR3F(v.x / dist,
		v.y / dist,
		v.z / dist);
}

VECTOR3F ConvertVec3(const float conv, VECTOR3F v)
{
	const VECTOR3F normalize_vec3 = NormalizeVec3(v);
	return normalize_vec3 * conv;
}

VECTOR3F LerpVec3(VECTOR3F lerpStart, VECTOR3F lerpEnd, float value)
{
	DirectX::XMVECTOR start, end, lerpPosition;
	VECTOR3F outPosition;
	start = DirectX::XMLoadFloat3(&lerpStart);
	end = DirectX::XMLoadFloat3(&lerpEnd);
	lerpPosition = DirectX::XMVectorLerp(start, end, value);
	DirectX::XMStoreFloat3(&outPosition, lerpPosition);
	return outPosition;
}

VECTOR3F SphereLinearVec3( VECTOR3F start, VECTOR3F end, float t)
{
	// 2ベクトル間の角度（鋭角側）
	float angle = acosf(DotVec3(start, end));
	if (!std::isfinite(angle))
		angle = 0.0f;
	// sinθ
	float SinTh = sinf(angle);

	// 補間係数
	float Ps = sinf(angle * (1.0f - t));
	float Pe = sinf(angle * t);

	VECTOR3F out;
	if (angle == 0.0f)
	{
		out.x = 0.0f;
		out.y = 0.0f;
		out.z = 0.0f;
	}
	else
	{
		out.x = (Ps * start.x + Pe * end.x) / SinTh;
		out.y = (Ps * start.y + Pe * end.y) / SinTh;
		out.z = (Ps * start.z + Pe * end.z) / SinTh;
	}

	return out;
}

VECTOR3F SphereLinearVec3(VECTOR3F origin,VECTOR3F start, VECTOR3F end, float t)
{
	VECTOR3F sphereLinerStart = NormalizeVec3(start - origin);
	VECTOR3F sphereLinerEnd = NormalizeVec3(end - origin);

	// 2ベクトル間の角度（鋭角側）
	float angle = acosf(DotVec3(sphereLinerStart, sphereLinerEnd));

	// sinθ
	float SinTh = sinf(angle);

	// 補間係数
	float Ps = sinf(angle * (1.0f - t));
	float Pe = sinf(angle * t);

	VECTOR3F out;
	if (angle == 0.0f)
	{
		out.x = 0.0f;
		out.y = 0.0f;
		out.z = 0.0f;
	}
	else
	{
		out.x = (Ps * sphereLinerStart.x + Pe * sphereLinerEnd.x) / SinTh;
		out.y = (Ps * sphereLinerStart.y + Pe * sphereLinerEnd.y) / SinTh;
		out.z = (Ps * sphereLinerStart.z + Pe * sphereLinerEnd.z) / SinTh;
	}

	return out;
}

VECTOR3F CrossVec3(const VECTOR3F v1, const VECTOR3F v2)
{
	return VECTOR3F(v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}

float DotVec3(const VECTOR3F v1, const VECTOR3F v2)
{
	float dot =  v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	if (!std::isfinite(dot))
		dot = 0.0f;

	return dot;
}