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

	bool canAttack = false;			//UŒ‚‚Å‚«‚é
	bool canPrepareAttack = false;	//UŒ‚€”õ‚Í‚Å‚«‚Ä‚¢‚é
	bool canFindAttackPoint = false;//UŒ‚’n“_‚ğ”­Œ©‚µ‚Ä‚¢‚é
	bool canMove = false;			//ˆÚ“®‚Å‚«‚é
	bool canRecover = false;		//‰ñ•œ‚Å‚«‚é
	bool canRevive = false;			//‘h¶‚Å‚«‚é
	
	bool isTargeted = false;		//•W“I‚É‚³‚ê‚Ä‚¢‚é
	bool hasDamaged = false;		//UŒ‚‚ğó‚¯‚Ä‚¢‚é
};
