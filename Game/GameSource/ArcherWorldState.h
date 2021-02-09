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
public:
    bool isSetArrow = false;

};