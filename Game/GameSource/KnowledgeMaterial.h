#pragma once

enum class DistanceLv
{
	NEAR_AWAY,			//�߂�
	FAR_WAY				//����
};
enum class HitPointLv
{
	FATAL,			//�v���I
	SEVERE_WOUND,	//�d��
//	MODERATE,		//����
	MINOR_INJURY,	//�y��
	UNINJURED,		//����
	END
};
enum class SkillLv
{
	DEXTERITY,		//���
	GENERAL,		//����
	UNSKILLFUL		//����
};