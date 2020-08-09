#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>
#include <unordered_map>
#include <string>
#include <assert.h>
#include <DirectXMath.h>


class Component;
class Entity;

using ComponentID = std::size_t;

inline ComponentID FindComponentTypeID()
{
	static ComponentID lastID = 0;
	return ++lastID;
}

template <typename T>
inline ComponentID FindComponentTypeID() noexcept
{
	static ComponentID typeID = FindComponentTypeID();
	return typeID;
}

constexpr std::size_t g_maxComponents = 32;

using ComponentBitSet = std::bitset<g_maxComponents>;
using ComponentArray = std::array<Component*, g_maxComponents>;

class Component
{
public:
	Entity* _entity;

	virtual bool Init() { return true; }
	virtual void Update(float& elapsedTime) {}
	virtual void Render() {}

	Component() : _entity(nullptr) {};
	virtual ~Component() = default;

};

class Entity
{
public:
	Entity() = default;;
	~Entity() = default;

	void Update(float& elapsedTime)
	{
		for (auto& c : m_component)
		{
			c->Update(elapsedTime);
		}
	}

	void Render()
	{
		for (auto& c : m_component)
		{
			c->Render();
		}
	}

	bool IsActive() { return m_isActivate; }
	void Destroy() { m_isActivate = false; }

	template <typename T>
	bool HasComponent()const
	{
		return m_componentBitSet[FindComponentTypeID<T>()];
	}

	template <typename T, typename...TArgs>
 	const T& AddComponent(TArgs&&... mArgs)
	{
		T* c(new T(std::forward<TArgs>(mArgs)...));

		c->_entity = this;
		std::unique_ptr<Component> uPtr{ c };
		m_component.emplace_back(std::move(uPtr));

		m_componentArray[FindComponentTypeID<T>()] = c;
		m_componentBitSet[FindComponentTypeID<T>()] = true;

		if (!c->Init())
			assert("Could not T initialize()");

		return *c;

	}

	template<typename T>
	T& FindComponent() const
	{
		auto ptr(m_componentArray[FindComponentTypeID<T>()]);
		return *static_cast<T*>(ptr);
	}

private:
	bool m_isActivate = true;
	std::vector<std::unique_ptr<Component>> m_component;

	ComponentArray m_componentArray = {};
	ComponentBitSet m_componentBitSet;
};

class EntityManager
{
public:
	void Update(float& elapsedTime)
	{
		for (auto& e : m_entities)
		{
			e.second->Update(elapsedTime);
		}
	}

	void Render()
	{
		for (auto& e : m_entities)
		{
			e.second->Render();
		}
	}

	void Relese()
	{
		for (auto e = m_entities.begin(); e != m_entities.end();)
		{
			if (e->second->IsActive())
				e = m_entities.erase(e);
			else
				++e;
		}
	}

	inline  Entity& FindEntity(std::size_t tag)
	{
		const auto it = m_entities.find(tag);
		if (it != m_entities.end())
		{
			return *it->second.get();
		}
		return *static_cast<Entity*>(nullptr);
	}

	Entity& AddEntity(std::size_t tag)
	{
		Entity* e = new Entity();
		std::unique_ptr<Entity> uPtr{ e };
		m_entities.insert(std::make_pair(tag, std::move(uPtr)));
		return *e;
	}

private:
	//	std::vector<std::unique_ptr<Entity>> m_entities;
	std::unordered_map<std::size_t, std::unique_ptr<Entity>> m_entities;
};

inline EntityManager& GetEntityManager()
{
	static 	EntityManager manager;
	return manager;
}