#include"SpriteData.h"
#include"Framework.h"

namespace Source
{
	namespace SpriteLoad
	{
		//	’Ç‰Á—p
//	{ TEXNO::,"Data/Texture/.png","../Asset/Texture/Noise.png",1U },
		LoadTexture loadTexture[] =
		{
			{ TextureLabel::TITLE,	"../Asset/Texture/Screen/title.png","../Asset/Texture/Noize/noize11.png",1U },
			{ TextureLabel::EFFECT0,"../Asset/Texture/Effect/fire.png","",1U },
			{ TextureLabel::SCOPE,	"../Asset/Texture/Scope/Scope.png" ,"",1U },
		};

		std::shared_ptr<Source::Sprite::SpriteBatch> SpriteLoader::GetTexture(TextureLabel texNo)
		{
			int num = static_cast<int>(texNo);
			if (m_texture[num] == nullptr)
			{
				m_texture[num] = std::make_shared<Source::Sprite::SpriteBatch>(Framework::GetInstance().GetDevice(), loadTexture[num].fileName, loadTexture[num].noiseFileName, loadTexture[num].maxInstance);
			}
			return m_texture[num];
		}
	}
}
