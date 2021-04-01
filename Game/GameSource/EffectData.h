#pragma once
#include <memory>
#include ".\LibrarySource\BillBoard.h"

enum EffectTextureLabel
{
	Heal_Effect,
	P_Effect,
	A_Effect,
	END
};

struct LoadEffectTexture
{
	EffectTextureLabel texNum;//テクスチャ番号
	const char* fileName;//ファイル名
};
extern LoadEffectTexture loadTexture[];

class EffectTextureLoader
{
public:
	EffectTextureLoader() = default;
	~EffectTextureLoader() = default;

	void LoadEffectTexture(const EffectTextureLabel label);
	void Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& projection, const FLOAT4X4& view,
		const EffectTextureLabel& label,const VECTOR2F& texPos,const VECTOR2F& texSize,
		const VECTOR3F& position,const float scale, const VECTOR3F& angle, const VECTOR4F& color);
	Source::BillBoard::BillBoard* GetBillBoard(const EffectTextureLabel label) { return m_billBoard[label].get(); }
	inline static EffectTextureLoader* GetInstance()
	{
		static EffectTextureLoader instance;
		return &instance;
	}

private:
	std::unique_ptr<Source::BillBoard::BillBoard> m_billBoard[EffectTextureLabel::END];
};

