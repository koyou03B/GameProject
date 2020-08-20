#pragma once

class Saber;
class Archer;
class Fighter;
class Enemy;


// 状態基底クラス
class State
{
public:
    virtual ~State() {}
    // ステートパターン
    virtual void Execute(Enemy* enemy) = 0;
    virtual void Execute(Saber* saber) = 0;
    virtual void Execute(Archer* archer) = 0;
    virtual void Execute(Fighter* fighte) = 0;
};
