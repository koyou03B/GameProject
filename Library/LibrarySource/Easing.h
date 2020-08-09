#pragma once

//https://easings.net/ja

class Easing
{
public:
	Easing() : m_time(0), m_totalTime(0), m_max(0), m_min(0) {}
	~Easing() = default;

	float Linear(float time, float totalTime, float max, float min);
	float InQuad(float time, float totalTime, float max, float min);
	float OutQuad(float time, float totalTime, float max, float min);
	float InOutQuad(float time, float totalTime, float max, float min);
	float InCubic(float time, float totalTime, float max, float min);
	float OutCubic(float time, float totalTime, float max, float min);
	float InOutCubic(float time, float totalTime, float max, float min);
	float InQuart(float time, float totalTime, float max, float min);
	float OutQuart(float time, float totalTime, float max, float min);
	float InOutQuart(float time, float totalTime, float max, float min);
	float InQuint(float time, float totalTime, float max, float min);
	float OutQuint(float time, float totalTime, float max, float min);
	float InOutQuint(float time, float totalTime, float max, float min);
	float OutBounce(float time, float totalTime, float max, float min);
	float InBounce(float time, float totalTime, float max, float min);
	float InOutBounce(float time, float totalTime, float max, float min);

	float InExp(float time, float totalTime, float max, float min);
	float OutExp(float time, float totalTime, float max, float min);
	float InOutExp(float time, float totalTime, float max, float min);
	float InCirc(float time, float totalTime, float max, float min);
	float OutCirc(float time, float totalTime, float max, float min);
	float InOutCirc(float time, float totalTime, float max, float min);
	float InBack(float time, float totalTime, float max, float min, float s);
	float OutBack(float time, float totalTime, float max, float min, float s);
	float InOutBack(float time, float totalTime, float max, float min, float s);


	inline static Easing& GetInstance()
	{
		static Easing instance;
		return instance;
	}

private:
	float m_time;
	float m_totalTime;
	float m_max;
	float m_min;
};

