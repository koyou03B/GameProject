#pragma once
#include <vector>
#include <memory>
#include "AnimationBlend.h"
#include ".\LibrarySource\Constants.h"
#include ".\LibrarySource\StaticMesh.h"
#include ".\LibrarySource\SkinnedMesh.h"


#include<cereal/types/memory.hpp>
#include<cereal/types/map.hpp>

struct ObjectWorldData
{
	VECTOR3F position;
	VECTOR3F angle;
	VECTOR3F scale;
	FLOAT4X4 world;

	void WorldUpdate()
	{
		DirectX::XMMATRIX S, R, T;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMStoreFloat4x4(&world, S * R * T);
	}

	void WorldClear()
	{
		position = {};
		angle = {};
		scale = {};
		world = {};
	}

	template<class T>
	void serialize(T& archive)
	{
		archive
		(
			position,
			angle,
			scale,
			world
		);
	};
};

struct ObjectSpriteData
{
	VECTOR2F position;
	VECTOR2F scale;
	VECTOR2F texPos;
	VECTOR2F texSize;

	std::unique_ptr<Source::ConstantBuffer::ConstantBuffer<Source::Constants::Dissolve>> m_dissolve;

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

	void SpriteClear()
	{
		position = {};
		texPos = {};
		scale = {};
		texSize = {};


		m_dissolve->~ConstantBuffer();
		m_dissolve.~unique_ptr();
		m_dissolve.release();
		
	}
};
enum ObjectElement 
{
	FIGHTER,
	ARCHER,
	JUGGERNAUT,
	STONE,
	STAGE,
	TITLE_LOG
};

enum ObjectType
{
	ACTOR,
	STATIC,
	SPRITE
};

class TitleObjectData
{
public:
	TitleObjectData(std::string name) { Init(name); };
	~TitleObjectData() = default;

	void Init(std::string name);
	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* immediateContext);
	void Release();
	void ImGui(ID3D11Device* device);
	void SetActModel(ObjectElement element);
	void SetStaticModel(ObjectElement element);
	void SetSprite();
	void SetAction(const bool isAction) { m_isAction =  isAction; }
	bool GetActionEnd() { return m_isActionEnd; }
	inline AnimationBlend& GetAnimation() { return m_animationBlend; }
	template<class T>
	void serialize(T& archive)
	{
		archive
		(
			m_worldData,
			m_name,
			m_spriteData
		);
	};
private:
	std::shared_ptr<Source::StaticMesh::StaticMesh>		m_staticModel;
	std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>	m_actorModel;
	std::shared_ptr<Source::Sprite::SpriteBatch>		m_sprite;
	ObjectWorldData m_worldData;
	ObjectSpriteData m_spriteData;
	AnimationBlend m_animationBlend;
	std::string m_name;
	bool m_isAction = false;
	bool m_isActionEnd = false;
};

#define STR(var) #var 
class TitleObjctAdominist
{
public:
	TitleObjctAdominist() { init(); }
	~TitleObjctAdominist() = default;

	void init();

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* immediateContext,ObjectElement element);

	void Release();

	void ImGui(ObjectElement element,ID3D11Device* device);

	void SetObject(ObjectElement element, ObjectType type);

	std::unique_ptr<TitleObjectData>& GetTitleObject(ObjectElement element)
	{
		std::map<ObjectElement, std::unique_ptr<TitleObjectData>>::iterator object = m_objects.find(element);
		if (object != m_objects.end())
		{
			if (object->second)
				return object->second;
		}

		return object->second;
	}

	inline std::string EnumToString(ObjectElement element)
	{
		switch (element)
		{
		case FIGHTER:
			return STR(FIGHTER);
			break;
		case ARCHER:
			return STR(ARCHER);
			break;
		case JUGGERNAUT:
			return STR(JUGGERNAUT);
			break;
		case STONE:
			return STR(STONE);
			break;
		case STAGE:
			return STR(STAGE);
			break;
		case TITLE_LOG:
			return STR(TITLE_LOG);
			break;
		default:
			break;
		}

		return "";
	}

	template<class T>
	void serialize(T& archive)
	{
		archive
		(
			m_objects
		);
	};

private:
	using Object =  std::unique_ptr<TitleObjectData>;
	std::map<ObjectElement, std::unique_ptr<TitleObjectData>>	m_objects;

};
