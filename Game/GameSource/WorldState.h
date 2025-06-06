#pragma once

#undef max
#undef min
#include<cereal/cereal.hpp>
#include<cereal/archives/binary.hpp>

#pragma region WorldStateについて
//自分の状態もふくめたゲームの世界全体の状態
//が、ここでは共通するものしか書かない
//継承先で自分のことを書いてもらう
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

	bool _canAttack = false;					//攻撃できる
	bool _canPrepareAttack = false;			//攻撃準備はできている
	bool _canFindAttackPoint = false;		//攻撃地点を発見している
	bool _canFindDirectionToAvoid = false;	//避ける方向を発見している
	bool _canAvoid = false;
	bool _canMove = false;					//移動できる
	bool _canRecover = false;				//回復できる
	bool _canRevive = false;					//蘇生できる
	
	bool _isTargeted = false;				//標的にされている
	bool _hasDamaged = false;				//攻撃を受けている

	int  _recoverMeter = 0;
	int  _attackMeter = 0;
	int  _avoidMeter = 0;
};
