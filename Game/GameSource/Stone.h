#pragma once
#include <vector>
#include <memory>
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\StaticMesh.h"
#include ".\LibrarySource\InstanceData.h"

class Stone
{
public:
	Stone() = default;
	~Stone() = default;

	void Init();

	void Update(float& elapsedTime);

	void Relase();

	VECTOR3F& GetSpeed() { return m_speed; }
	VECTOR3F& GetVelocity() { return m_velocity; }
	float& GetAttack() { return m_attack; }
	float GetTimer() { return m_timer; }
	bool GetExit() { return m_isExit; }

	void SetSpeed(const VECTOR3F& speed) { m_speed = speed; }
	void SetVelocity(const VECTOR3F& velocity) { m_velocity = velocity; }
	void SetAttack(const float& attack) { m_attack = attack; }
	void SetTimer(const float& timer) { m_timer = timer; }
	void SetExit(const bool& flg) { m_isExit = flg; }


protected:
	const float Gravity = -9.8f;

	VECTOR3F m_speed = {};
	VECTOR3F m_velocity = {};
	float m_timer = 0;
	float m_attack = 0.0f;
	bool m_isExit = false;
};

class StoneAdominist
{
public:
	StoneAdominist() { Init(); };
	~StoneAdominist() { Release(); };

public:
	void Init();

	void Update(float& elapsedTime);

	void Render(ID3D11DeviceContext* immediateContext);

	void ImGui(ID3D11Device* device);

	void Release()
	{
		if (m_model.unique())
			m_model.reset();
		
		if (!m_instanceData.empty())
			m_instanceData.clear();

		if (m_stone.unique())
		{
			m_stone->Relase();
			m_stone.reset();
		}
	}

	void Reset()
	{
		m_stone->SetExit(false);
		m_power = 20.0f;
		m_isFly = false;
	}

	void ReleaseArrowParm()
	{
		if (!m_stone) return;
		m_stone->Relase();
		m_isFly = false;
	}

	std::shared_ptr<Stone>& GetStone() { return m_stone; }
	std::vector<Source::InstanceData::InstanceData>& GetInstanceData() { return m_instanceData; }
	bool GetIsFly() { return m_isFly; }
	inline float GetPower() { return m_power; }
	void SetStone(const VECTOR3F& position, const VECTOR3F& angle, const VECTOR3F& velocity);

	void SetFly(bool isFly) {	m_isFly = isFly; }



	inline static StoneAdominist& GetInstance()
	{
		static 	StoneAdominist arrowAdomin;
		return arrowAdomin;
	}

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 1)
		{
			archive
			(
				m_survivalTime,
				m_scale
			);
		}
	};

private:
	std::shared_ptr<Source::StaticMesh::StaticMesh> m_model;
	std::shared_ptr<Stone> m_stone;
	std::vector<Source::InstanceData::InstanceData> m_instanceData;

	int m_survivalTime = 0;
	float m_power = 20.0f;
	VECTOR3F m_scale;
	bool m_isFly = false;
};

struct StoneParameter
{
	bool m_isExist;
	VECTOR3F offsetZ;
	VECTOR3F offsetX;
	std::unique_ptr<StoneAdominist> m_stoneAdom;

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 1)
		{
			archive
			(
				offsetZ,
				offsetX,
				m_stoneAdom
			);
		}
		else
		{
			archive
			(
				offsetZ,
				offsetX,
				m_stoneAdom

			);
		}
	}
};

//#define StoneInstamce StoneAdominist::GetInstance()