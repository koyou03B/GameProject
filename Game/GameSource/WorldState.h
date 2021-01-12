#pragma once

#undef max
#undef min
#include<cereal/cereal.hpp>
#include<cereal/archives/binary.hpp>

#pragma region WorldState�ɂ���
//�����̏�Ԃ��ӂ��߂��Q�[���̐��E�S�̂̏��
//���A�����ł͋��ʂ�����̂��������Ȃ�
//�p����Ŏ����̂��Ƃ������Ă��炤
#pragma endregion

class WorldState
{
public:
	WorldState() = default;
	~WorldState() = default;

	enum Status
	{
		CanAttack,	//�U���ł���
		CanMove,	//�ړ��ł���
		CanAvoid,	//����ł���
		CanRecover,	//�񕜂ł���
		CanRevive,	//�h���ł���
		MaxStatus
	};

	inline bool GetStatus(const Status status)
	{
		return m_hasBecome[static_cast<int>(status)];
	}

	inline void SetStatus(const Status status, const bool value)
	{
		m_hasBecome[static_cast<int>(status)] = value;
	}

protected:
	bool m_hasBecome[Status::MaxStatus];
};