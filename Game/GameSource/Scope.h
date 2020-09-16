#pragma once
#include <memory>
#include <iostream>
#include <fstream>
#include <shlwapi.h>
#include <string>
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\Framework.h"
#include ".\LibrarySource\SpriteData.h"

class Scope
{
public:
	Scope() = default;
	~Scope() = default;

	bool Init()
	{
		m_scopeTexture = TEXTURELOADER.GetTexture(Source::SpriteLoad::TextureLabel::SCOPE);
		m_texturePosition = VECTOR2F(Framework::GetInstance().SCREEN_WIDTH * 0.5f, Framework::GetInstance().SCREEN_HEIGHT * 0.5f);
		m_textureScale = VECTOR2F(512, 512);
		m_textureSize = VECTOR2F(512, 512);

		if (PathFileExistsA((std::string("../Asset/Binary/Player/Archer/Scope") + ".bin").c_str()))
		{
			std::ifstream ifs;
			ifs.open((std::string("../Asset/Binary/Player/Archer/Scope") + ".bin").c_str(), std::ios::binary);
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(*this);
		}

		return true;
	}

	void Update(VECTOR3F target);

	void Render(ID3D11DeviceContext* immediateContext)
	{
		m_scopeTexture->Begin(immediateContext);
		m_scopeTexture->RenderCenter(m_texturePosition, m_textureScale,
			VECTOR2F(0, 0), m_textureSize, 0, m_textureColor, false);
		m_scopeTexture->End(immediateContext);
	}

	void ImGui(ID3D11Device* device);

	bool GetAimMode() { return m_isAimMode; }

	void SetAimMode() { m_isAimMode = !m_isAimMode; }
		
	void SaveBinary()
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/Player/Archer/Scope") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}
	
	template<class T>
	void serialize(T& archive)
	{
		archive
		(
			m_yAxisValue,
			m_textureScale,
			m_textureColor
		);
	}

private:

	std::shared_ptr<Source::Sprite::SpriteBatch> m_scopeTexture;

	bool m_isAimMode = false;
	float m_yAxisValue = 0.0f;

	VECTOR2F m_texturePosition = {};
	VECTOR2F m_textureScale = {};
	VECTOR2F m_textureSize = {};
	VECTOR4F m_textureColor = { 1.0f,1.0f,1.0f,1.0f };

};