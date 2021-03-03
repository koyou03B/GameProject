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
	virtual ~WorldState() = default;

	void MeterReset()
	{
		_recoverMeter = 0;
		_attackMeter = 0;
		_avoidMeter = 0;
	}

	bool _canAttack = false;					//�U���ł���
	bool _canPrepareAttack = false;			//�U�������͂ł��Ă���
	bool _canFindAttackPoint = false;		//�U���n�_�𔭌����Ă���
	bool _canFindDirectionToAvoid = false;	//����������𔭌����Ă���
	bool _canAvoid = false;
	bool _canMove = false;					//�ړ��ł���
	bool _canRecover = false;				//�񕜂ł���
	bool _canRevive = false;					//�h���ł���
	
	bool _isTargeted = false;				//�W�I�ɂ���Ă���
	bool _hasDamaged = false;				//�U�����󂯂Ă���

	int  _recoverMeter = 0;
	int  _attackMeter = 0;
	int  _avoidMeter = 0;
};
