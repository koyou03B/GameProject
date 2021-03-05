#pragma once
#include <vector>
#include <memory>
#include <map>
#include<cereal/types/memory.hpp>
#include<cereal/types/vector.hpp>
#include<cereal/types/map.hpp>
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\SpriteData.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif

struct SpriteParam
{
	VECTOR2F position;
	VECTOR2F scale;
	VECTOR2F texPos;
	VECTOR2F texSize;
	float maxTexSizeX;
	float currentMaxSize;
	void ParameterClear()
	{
		position = {};
		texPos = {};
		scale = {};
		texSize = {};
		maxTexSizeX = 0.0f;
		currentMaxSize = 0.0f;
	}

	template<class T>
	void serialize(T& archive)
	{
		archive
		(
			position,
			scale,
			texPos,
			texSize
		);
	};

};

enum UIStaticLabel
{
	LIFE_GAGE,
	COMMAND_F,
	NAME_F,
	NAME_E,
	NAME_A,
	ENDS
};

enum UIActLabel
{
	LIFE_P,
	LIFE_E,
	ENDA
};

class UIData
{
public:
	UIData() = default;
	~UIData() = default;

	void Init(std::string name);
	void Render(ID3D11DeviceContext* immediateContext);
	void Release();
	void ImGui();

	void LifeUpdate(float elapsedTime);

	bool GetHasUpdate() { return m_hasUpdate; }
	void SetHasUpdate(const bool isUpdate) { m_hasUpdate = isUpdate; }
	void FindValue(float value,const int id);

	void SetSpriteS(UIStaticLabel label);
	void SetSpriteA(UIActLabel label);

	template<class T>
	void serialize(T& archive)
	{
		archive
		(
			m_spriteParam
		);
	};
private:
	std::vector<SpriteParam> m_spriteParam;
	std::shared_ptr<Source::Sprite::SpriteBatch> m_sprite;
	std::string m_binaryName;
	bool m_hasUpdate=false;
	float m_updateValue = 0.0f;
	float m_updateTime = 0.0f;
	int m_instanceNo = 0;
};
#define STR(var) #var 
class UIAdominist
{
public:
	UIAdominist() = default;
	~UIAdominist() = default;

	void Init();
	void Update(float& elapsedTime);
	void Render(ID3D11DeviceContext* immediateContext);
	void Release();
	void ImGui();

	void LifeUpdate(const UIActLabel& label,const float& updateValue,const int id)
	{
		std::map<UIActLabel, UIData>::iterator object = m_uiActData.find(label);
		if (object != m_uiActData.end())
		{
			object->second.SetHasUpdate(true);
			object->second.FindValue(updateValue, id);
		}
	}

	template<class T>
	void serialize(T& archive)
	{
		archive
		(
			m_uiStaticData,
			m_uiActData
		);
	};

private:
	inline std::string EnumToStringS(UIStaticLabel label)
	{
		switch (label)
		{
		case LIFE_GAGE:
			return STR(LIFE_GAGE);
			break;
		case COMMAND_F:
			return STR(COMMAND_F);
			break;
		case NAME_F:
			return STR(NAME_F);
			break;
		case NAME_E:
			return STR(NAME_E);
			break;
		}

		return "";
	}
	inline std::string EnumToStringA(UIActLabel label)
	{
		switch (label)
		{
		case LIFE_P:
			return STR(LIFE_P);
			break;
		case LIFE_E:
			return STR(LIFE_E);
			break;
		}

		return "";
	}
private:
	std::map<UIStaticLabel, UIData>		m_uiStaticData;
	std::map<UIActLabel, UIData>		m_uiActData;
};