#include "AnimationData.h"

void AnimationData::Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& projection, const FLOAT4X4& view,
	const VECTOR3F& position, const float& scale, const VECTOR3F& angle, const VECTOR4F& color)
{
	auto effectLoader = EffectTextureLoader::GetInstance();
	effectLoader->Render(immediateContext, projection, view,
		this->label, this->tilePosition, this->tileSize,
		position, scale, angle, color);
}
