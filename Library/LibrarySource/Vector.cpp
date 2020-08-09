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
VECTOR2F SphereLinearVec2(VECTOR2F* start, VECTOR2F* end, float t)
{
	VECTOR2F sphereLinerStart = NormalizeVec2(*start);
	VECTOR2F sphereLinerEnd = NormalizeVec2(*end);

	// 2ベクトル間の角度（鋭角側）
	float angle = acosf(DotVec2(sphereLinerStart, sphereLinerEnd));

	// sinθ
	float SinTh = sinf(angle);

	// 補間係数
	float Ps = sinf(angle * (1.0f - t));
	float Pe = sinf(angle * t);

	//VECTOR2F out = (Ps * sphereLinerStart + Pe * sphereLinerEnd) / SinTh;
	VECTOR2F out;
	out.x = (Ps * sphereLinerStart.x + Pe * sphereLinerEnd.x) / SinTh;
	out.y = (Ps * sphereLinerStart.y + Pe * sphereLinerEnd.y) / SinTh;

	// 一応正規化して球面線形補間に
	return NormalizeVec2(out);
}
;

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

VECTOR3F SphereLinearVec3(VECTOR3F* start, VECTOR3F* end, float t)
{
	VECTOR3F sphereLinerStart = NormalizeVec3(*start);
	VECTOR3F sphereLinerEnd = NormalizeVec3(*end);

	// 2ベクトル間の角度（鋭角側）
	float angle = acosf(DotVec3(sphereLinerStart, sphereLinerEnd));

	// sinθ
	float SinTh = sinf(angle);

	// 補間係数
	float Ps = sinf(angle * (1.0f - t));
	float Pe = sinf(angle * t);

	//VECTOR2F out = (Ps * sphereLinerStart + Pe * sphereLinerEnd) / SinTh;
	VECTOR3F out;
	out.x = (Ps * sphereLinerStart.x + Pe * sphereLinerEnd.x) / SinTh;
	out.y = (Ps * sphereLinerStart.y + Pe * sphereLinerEnd.y) / SinTh;
	out.z = (Ps * sphereLinerStart.z + Pe * sphereLinerEnd.z) / SinTh;

	// 一応正規化して球面線形補間に
	return NormalizeVec3(out);
}

VECTOR3F CrossVec3(const VECTOR3F v1, const VECTOR3F v2)
{
	return VECTOR3F(v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}

float DotVec3(const VECTOR3F v1, const VECTOR3F v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}