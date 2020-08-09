#include"SpriteData.h"
#include"Framework.h"

namespace Source
{
	namespace SpriteData
	{
		//	’Ç‰Á—p
//	{ TEXNO::,"Data/Texture/.png","../Asset/Texture/Noise.png",1U },
		LoadTexture loadTexture[] =
		{
			{ TextureLabel::TITLE,	"../Asset/Texture/title.png","../Asset/Texture/noize/noize11.png",true,1U },
			{ TextureLabel::SELECT,	"../Asset/Texture/MARK.png","",false,1U },
			{ TextureLabel::LOAD,		"../Asset/Texture/n64.png" ,"",false,1U },
			{ TextureLabel::LOADICON,	"../Asset/Texture/n64.png" ,"",false,1U },
		};

		void SpriteData::Load(ID3D11Device* device)
		{
			for (int i = 0; i < static_cast<int>(TextureLabel::END); ++i)
			{
				//_texture[i] = std::make_shared<SpriteBatch>(loadTexture[i].fileName, loadTexture[i].maxInstance);
				if (!loadTexture[i].isDissolve)
					m_texture[i] = std::make_shared<Source::Sprite::SpriteBatch>(device, loadTexture[i].fileName, loadTexture[i].maxInstance);
				else
					m_texture[i] = std::make_shared<Source::Sprite::SpriteBatch>(device, loadTexture[i].fileName, loadTexture[i].noiseFileName, loadTexture[i].maxInstance);
			}
		}

		void SpriteData::RenderC(ID3D11DeviceContext* immediateContext, int texNo,
			const VECTOR2F& position, const VECTOR2F& scale,
			const VECTOR2F& texPos, const VECTOR2F& texSize,
			float angle, const VECTOR4F& color, bool reverse)
		{
			if ((size_t)texNo < static_cast<int>(TextureLabel::END))
			{
				m_texture[texNo]->Begin(immediateContext);
				m_texture[texNo]->RenderCenter(position, scale, texPos, texSize, angle, color, reverse);
				m_texture[texNo]->End(immediateContext);
			}
		}



		void SpriteData::RenderL(ID3D11DeviceContext* immediateContext, int texNo,
			const VECTOR2F& position, const VECTOR2F& scale,
			const VECTOR2F& texPos, const VECTOR2F& texSize,
			float angle, const VECTOR4F& color, bool reverse)
		{
			if ((size_t)texNo < static_cast<int>(TextureLabel::END))
			{
				m_texture[texNo]->Begin(immediateContext);
				m_texture[texNo]->RenderLeft(position, scale, texPos, texSize, angle, color, reverse);
				m_texture[texNo]->End(immediateContext);

			}
		}

	}
}
