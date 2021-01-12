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
	~WorldState() = default;

	enum Status
	{
		CanAttack,	//UŒ‚‚Å‚«‚é
		CanMove,	//ˆÚ“®‚Å‚«‚é
		CanAvoid,	//‰ñ”ğ‚Å‚«‚é
		CanRecover,	//‰ñ•œ‚Å‚«‚é
		CanRevive,	//‘h¶‚Å‚«‚é
		MaxStatus
	};

	inline bool GetStatus(const Status status)
	{
		return m_hasBecome[static_cast<int>(status)];
	}

	inline void SetStatus(const Status status, const bool value)
	{
		m_hasBecome[static_cast<int>(status)] = value;
	}

protected:
	bool m_hasBecome[Status::MaxStatus];
};