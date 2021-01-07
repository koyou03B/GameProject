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

    enum ArcherStatus
    {
        HoldingArrow,   //������܂��Ă���
        Max_ArcherStatus
    };

    inline bool GetStatus(const int status)
    {
        return m_aHasBecame[status];
    }

    inline void SetStatus(const int status,bool value)
    {
        m_aHasBecame[status] = value;
    }
private:
    bool m_aHasBecame[Max_ArcherStatus]; 
};