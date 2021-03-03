#pragma once

#undef max
#undef min
#include<cereal/cereal.hpp>
#include<cereal/archives/binary.hpp>

#pragma region WorldState‚É‚Â‚¢‚Ä
//©•ª‚Ìó‘Ô‚à‚Ó‚­‚ß‚½ƒQ[ƒ€‚Ì¢ŠE‘S‘Ì‚Ìó‘Ô
//‚ªA‚±‚±‚Å‚Í‹¤’Ê‚·‚é‚à‚Ì‚µ‚©‘‚©‚È‚¢
//Œp³æ‚Å©•ª‚Ì‚±‚Æ‚ğ‘‚¢‚Ä‚à‚ç‚¤
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

	bool _canAttack = false;					//UŒ‚‚Å‚«‚é
	bool _canPrepareAttack = false;			//UŒ‚€”õ‚Í‚Å‚«‚Ä‚¢‚é
	bool _canFindAttackPoint = false;		//UŒ‚’n“_‚ğ”­Œ©‚µ‚Ä‚¢‚é
	bool _canFindDirectionToAvoid = false;	//”ğ‚¯‚é•ûŒü‚ğ”­Œ©‚µ‚Ä‚¢‚é
	bool _canAvoid = false;
	bool _canMove = false;					//ˆÚ“®‚Å‚«‚é
	bool _canRecover = false;				//‰ñ•œ‚Å‚«‚é
	bool _canRevive = false;					//‘h¶‚Å‚«‚é
	
	bool _isTargeted = false;				//•W“I‚É‚³‚ê‚Ä‚¢‚é
	bool _hasDamaged = false;				//UŒ‚‚ğó‚¯‚Ä‚¢‚é

	int  _recoverMeter = 0;
	int  _attackMeter = 0;
	int  _avoidMeter = 0;
};
