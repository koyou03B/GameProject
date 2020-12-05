#pragma once
#include<memory>
#include<vector>
#include"d3d11.h"
#include"Vector.h"
#include"SpriteBatch.h"
#include"Constants.h"

namespace Source
{
	namespace SpriteLoad
	{
		enum class TextureLabel
		{
			TITLE,
			CLEAR,
			OVER,
			FIGHTER,
			JUGGERNAUT,
			P_LIFE,
			E_LIFE,
			P_LIFE_GAGE,
			COMMAND,
			EFFECT0,
			SCOPE,
			END,
		};

		struct LoadTexture
		{
			TextureLabel texNum;//テクスチャ番号
			const char* fileName;//ファイル名
			const char* noiseFileName;//ファイル名
			UINT maxInstance; //最大数
		};

		class SpriteLoader
		{
		public:
			SpriteLoader() = default;
			~SpriteLoader() = default;

			std::shared_ptr<Source::Sprite::SpriteBatch> GetTexture(TextureLabel texNo);

		private:
			std::shared_ptr<Source::Sprite::SpriteBatch> m_texture[static_cast<int>(TextureLabel::END)];

		};

		extern LoadTexture loadTexture[];


		inline Source::SpriteLoad::SpriteLoader& TextureLode()
		{
			static 	Source::SpriteLoad::SpriteLoader texLoder;
			return texLoder;
		}
	}
}

#define TEXTURELOADER Source::SpriteLoad::TextureLode()