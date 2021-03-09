#include "EffectData.h"
#include ".\LibrarySource\Framework.h"

LoadEffectTexture loadTexture[] =
{
	{ EffectTextureLabel::GREEN_SPLASH,		"../Asset/Texture/Title/TITLE.png"},
	{ EffectTextureLabel::WHITE_SPLASH,		"../Asset/Texture/Clear/CLEAR_LOG.png"},
	{ EffectTextureLabel::GREEN_WHITE_SPLASH,		"../Asset/Texture/Over/OVER_LOG.png"},
	{ EffectTextureLabel::RED_SPLASH,	"../Asset/Texture/Game/FighterName.png"},
};

void EffectTextureLoader::LoadEffectTexture(const EffectTextureLabel label)
{
	if(m_billBoard[label] == nullptr)
		m_billBoard[label] = std::make_unique<Source::BillBoard::BillBoard>(Framework::GetInstance().GetDevice(), loadTexture[label].fileName,false);

}

void EffectTextureLoader::Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& projection, const FLOAT4X4& view,
	const EffectTextureLabel& label, const VECTOR2F& texPos, const VECTOR2F& texSize,
	const VECTOR3F& position, const float scale, const VECTOR3F& angle, const VECTOR4F& color)
{
	m_billBoard[label]->Begin(immediateContext);
	m_billBoard[label]->TileCutRender(immediateContext, texPos, texSize);
	m_billBoard[label]->Render(immediateContext, projection, view, position, scale, angle, color);
	m_billBoard[label]->End(immediateContext);
}
