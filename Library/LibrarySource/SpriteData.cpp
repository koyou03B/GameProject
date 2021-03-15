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
			{ TextureLabel::TITLE,		"../Asset/Texture/Title/TITLE.png","../Asset/Texture/Noize/TitleLog.png",1U },
			{ TextureLabel::CLEAR,		"../Asset/Texture/Clear/CLEAR_LOG.png","",1U },
			{ TextureLabel::OVER,		"../Asset/Texture/Over/OVER_LOG.png","",1U },
			{ TextureLabel::FIGHTER,	"../Asset/Texture/Game/FighterName.png","",1U },
			{ TextureLabel::JUGGERNAUT,	"../Asset/Texture/Game/EnemyName.png","",1U },
			{ TextureLabel::ARCHER,		"../Asset/Texture/Game/ArcherName.png","",1U },
			{ TextureLabel::P_LIFE,		"../Asset/Texture/Game/PlayerLife.png","",2U },
			{ TextureLabel::E_LIFE,		"../Asset/Texture/Game/EnemyLife.png","",1U },
			{ TextureLabel::P_LIFE_GAGE,"../Asset/Texture/Game/LifeGage.png","",1U },
			{ TextureLabel::COMMAND,	"../Asset/Texture/Game/Command4.png","",1U },
			{ TextureLabel::EFFECT0,	"../Asset/Texture/Effect/fire.png","",1U },
			{ TextureLabel::SCOPE,		"../Asset/Texture/Scope/Scope.png" ,"",1U },
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
