#pragma once
#include <vector>
#include <memory>
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\StaticMesh.h"
#include ".\LibrarySource\InstanceData.h"

class Arrow
{
public:
	Arrow() = default;
	~Arrow() = default;

	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual void Relase() = 0;

	VECTOR3F& GetSpeed()										{ return m_speed; }
	VECTOR3F& GetVelocity()										{ return m_velocity; }
	float& GetAttack()											{ return m_attack; }
	int GetTimer()												{ return m_timer; }
	bool GetExit()												{ return m_isExit; }

	void SetSpeed(const VECTOR3F& speed)						{ m_speed = speed; }
	void SetVelocity(const VECTOR3F& velocity)					{ m_velocity = velocity; }
	void SetAttack(const float& attack)							{ m_attack = attack; }
	void SetTimer(const int& timer)								{ m_timer = timer; }
	void SetExit(const bool& flg)								{ m_isExit = flg; }


protected:
	const float Gravity = -9.8f;

	VECTOR3F m_speed;
	VECTOR3F m_velocity;
	float m_attack;
	int m_timer;
	bool m_isExit;
};

class NormalArrow : public Arrow
{
public:
	NormalArrow() = default;
	~NormalArrow() = default;

	void Init();

	void Update();

	void Relase();

	template<class T>
	void serialize(T& archive)
	{
		archive
		(
			m_speed,
			m_velocity,
			m_attack,
			m_timer
		);
	};
};

class StrongArrow : public Arrow
{
public:
	StrongArrow() = default;
	~StrongArrow() = default;

	void Init();

	void Update();

	void Relase();

	template<class T>
	void serialize(T& archive)
	{
		archive
		(
			m_speed,
			m_velocity,
			m_attack,
			m_timer,
			m_maxArrow
		);
	};

private:
	int m_maxArrow;
};

class ArrowAdominist
{
public:
	void Init();

	void Update(float& elapsedTime) ;

	void Render(ID3D11DeviceContext* immediateContext);

	void ImGui(ID3D11Device* device);

	void Release()
	{
		if (m_model.unique())
		{
			m_model.reset();
		}

		if (!m_instanceData.empty())
			m_instanceData.clear();
	}

	void ReleaseArrowParm()
	{
		if (!m_choisArrow) return;
		m_choisArrow->Relase();
		m_isShot = false;
	}

	std::shared_ptr<Arrow>& GetArrow() { return m_choisArrow; }
	std::vector<Source::InstanceData::InstanceData>& GetInstanceData() { return m_instanceData; }
	bool GetIsShot() { return m_isShot; }

	void SetArrow(const VECTOR3F& position,const VECTOR3F& angle,const VECTOR3F& velocity);

	void SetShot(bool isShot) { m_isShot = isShot; }

	inline static ArrowAdominist& GetInstance()
	{
		static 	ArrowAdominist arrowAdomin;
		return arrowAdomin;
	}

	template<class T>
	void serialize(T& archive)
	{
		archive
		(
			m_normalArrow,
			m_strongArrow,
			m_survivalTime,
			m_scale
		);
	};

private:
	std::shared_ptr<Source::StaticMesh::StaticMesh> m_model;

	std::shared_ptr<NormalArrow> m_normalArrow;
	std::shared_ptr<StrongArrow> m_strongArrow;

	std::shared_ptr<Arrow> m_choisArrow;
	std::vector<Source::InstanceData::InstanceData> m_instanceData;

	int m_survivalTime;
	VECTOR3F m_scale = {.5f,.5f,.5f};
	VECTOR3F m_target = {};
	bool m_isShot = false;
};

#define ArrowInstamce ArrowAdominist::GetInstance()