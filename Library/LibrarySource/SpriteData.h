#pragma once
#include<memory>
#include<vector>
#include"d3d11.h"
#include"Vector.h"
#include"SpriteBatch.h"
#include"Constants.h"

namespace Source
{
	namespace SpriteData
	{
		enum class TextureLabel
		{
			TITLE,
			SELECT,
			LOAD,
			LOADICON,
			END,
		};

		struct LoadTexture
		{
			TextureLabel texNum;//テクスチャ番号
			const char* fileName;//ファイル名
			const char* noiseFileName;//ファイル名
			bool isDissolve;
			UINT maxInstance; //最大数
		};

		class SpriteData
		{
		public:
			SpriteData() = default;
			~SpriteData() = default;

			void Load(ID3D11Device* device);

			void RenderC(ID3D11DeviceContext* immediateContext, int texNo,
				const VECTOR2F& position, const VECTOR2F& scale,
				const VECTOR2F& texPos, const VECTOR2F& texSize,
				float angle, const VECTOR4F& color, bool reverse);


			void RenderL(ID3D11DeviceContext* immediateContext, int texNo,
				const VECTOR2F& position, const VECTOR2F& scale,
				const VECTOR2F& texPos, const VECTOR2F& texSize,
				float angle, const VECTOR4F& color, bool reverse);


			std::shared_ptr<Source::Sprite::SpriteBatch> GetSprite(TextureLabel texNo) { return m_texture[static_cast<int>(texNo)]; }


		private:
			std::shared_ptr<Source::Sprite::SpriteBatch> m_texture[static_cast<int>(TextureLabel::END)];

		};

		extern LoadTexture loadTexture[];
	}
}

