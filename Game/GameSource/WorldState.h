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

	bool canAttack = false;			//�U���ł���
	bool canPrepareAttack = false;	//�U�������͂ł��Ă���
	bool canFindAttackPoint = false;//�U���n�_�𔭌����Ă���
	bool canMove = false;			//�ړ��ł���
	bool canRecover = false;		//�񕜂ł���
	bool canRevive = false;			//�h���ł���
	
	bool isTargeted = false;		//�W�I�ɂ���Ă���
	bool hasDamaged = false;		//�U�����󂯂Ă���
};
