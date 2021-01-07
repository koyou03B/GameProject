#pragma once
#include "WorldState.h"

#pragma region 自身のWorldStateについて
//Archer専用WorldState
//EffectやPrecondition側では
//int型を保持してもらう。
//Fighter用WorldStateが作られたときに
//共通して値の保持をしてもらうには
//int型が有効と判断
#pragma endregion
class ArcherWorldState : public WorldState
{
public:
    ArcherWorldState() = default;
    ~ArcherWorldState() = default;

    enum ArcherStatus
    {
        HoldingArrow,   //矢をかまえている
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