#pragma once
#include "WorldState.h"

#pragma region ���g��WorldState�ɂ���
//Archer��pWorldState
//Effect��Precondition���ł�
//int�^��ێ����Ă��炤�B
//Fighter�pWorldState�����ꂽ�Ƃ���
//���ʂ��Ēl�̕ێ������Ă��炤�ɂ�
//int�^���L���Ɣ��f
#pragma endregion

class ArcherWorldState : public WorldState
{
public:
    ArcherWorldState() = default;
    ~ArcherWorldState() = default;
public:
    bool isSetArrow = false;

};