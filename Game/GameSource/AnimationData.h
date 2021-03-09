#pragma once
#include <d3d11.h>
#include "EffectData.h"
#include ".\LibrarySource\Vector.h"

#undef max
#undef min
#include<cereal/cereal.hpp>
#include<cereal/archives/binary.hpp>


struct AnimationData
{
	EffectTextureLabel label;
	VECTOR2F tilePosition;
	VECTOR2F tileSize;
	VECTOR2F tileCount;
	float endTime;
	int tileMaxCount;
	void Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& projection, const FLOAT4X4& view,
		const VECTOR3F& position, const float& scale, const VECTOR3F& angle, const VECTOR4F& color);
	
	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 0)
		{
			archive
			(
				label,
				tileSize,
				tileCount,
				endTime,
				tileMaxCount
			);
		}
		else
		{
			archive
			(
				label,
				tileSize,
				tileCount,
				endTime,
				tileMaxCount
			);
		}
	}
};