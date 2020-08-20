#pragma once

class Saber;
class Archer;
class Fighter;
class Enemy;


// ��Ԋ��N���X
class State
{
public:
    virtual ~State() {}
    // �X�e�[�g�p�^�[��
    virtual void Execute(Enemy* enemy) = 0;
    virtual void Execute(Saber* saber) = 0;
    virtual void Execute(Archer* archer) = 0;
    virtual void Execute(Fighter* fighte) = 0;
};
