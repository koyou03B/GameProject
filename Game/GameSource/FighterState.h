#pragma once
#include "State.h"
class Fighter;


//-----------------------------------------------------------------
//Å@It's Move state
//-----------------------------------------------------------------
class MovekState : public State
{
private:
    MovekState() {}

public:
	 inline static MovekState* GetInstance()
	{
		static MovekState moveState;
		return &moveState;
	};
	 void Execute(Enemy* enemy) {};
	 void Execute(Saber* saber) {};
	 void Execute(Archer* archer) {};
	 void Execute(Fighter* fighte);
};
