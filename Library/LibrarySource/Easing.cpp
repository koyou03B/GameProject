#include <DirectXMath.h>
#include "Easing.h"


float Easing::OutCubic(float time, float totalTime, float max, float min)
{
	m_time = time;
	m_totalTime = totalTime; 
	m_max = max;
	m_min = min;
	m_max -= m_min;
	m_time = m_time / m_totalTime - 1;

	return m_max * (m_time * m_time * m_time + 1) + m_min;
}

float Easing::InQuint(float time, float totalTime, float max, float min)
{
	max -= min;
	time /= totalTime;
	return max * time * time * time * time * time + min;
}
float Easing::Linear(float time, float totalTime, float max, float min)
{
	return (max - min) * time / totalTime + min;
}

float Easing::InQuad(float time, float totalTime, float max, float min)
{
	max -= min;
	time /= totalTime;
	return max * time * time + min;
}
float Easing::OutQuad(float time, float totalTime, float max, float min)
{
	max -= min;
	time /= totalTime;
	return -max * time * (time - 2) + min;
}
float Easing::InOutQuad(float time, float totalTime, float max, float min)
{
	max -= min;
	time /= totalTime;
	if (time / 2 < 1)
		return max / 2 * time * time + min;
	--time;
	return -max * (time * (time - 2) - 1) + min;
}
float Easing::InCubic(float time, float totalTime, float max, float min)
{
	max -= min;
	time /= totalTime;
	return max * time * time * time + min;
}
float Easing::InOutCubic(float time, float totalTime, float max, float min)
{
	max -= min;
	time /= totalTime;
	if (time / 2 < 1)
		return max / 2 * time * time * time + min;
	time -= 2;
	return max / 2 * (time * time * time + 2) + min;
}
float Easing::InQuart(float time, float totalTime, float max, float min)
{
	max -= min;
	time /= totalTime;
	return max * time * time * time * time + min;
}
float Easing::OutQuart(float time, float totalTime, float max, float min)
{
	m_time = time;
	m_totalTime = totalTime;
	m_max = max; 
	m_min = min;
	m_max -= m_min;
	m_time = m_time / m_totalTime - 1;
	return -m_max * (m_time * m_time * m_time * m_time - 1) + m_min;
}
float Easing::InOutQuart(float time, float totalTime, float max, float min)
{
	max -= min;
	time /= totalTime;
	if (time / 2 < 1)
		return max / 2 * time * time * time * time + min;
	time -= 2;
	return -max / 2 * (time * time * time * time - 2) + min;
}
float Easing::OutQuint(float time, float totalTime, float max, float min)
{
	max -= min;
	time = time / totalTime - 1;
	return max * (time * time * time * time * time + 1) + min;
}
float Easing::InOutQuint(float time, float totalTime, float max, float min)
{
	max -= min;
	time /= totalTime;
	if (time / 2 < 1)
		return max / 2 * time * time * time * time * time + min;
	time -= 2;
	return max / 2 * (time * time * time * time * time + 2) + min;
}

float Easing::OutBounce(float time, float totalTime, float max, float min)
{
	m_time = time; 
	m_totalTime = totalTime; 
	m_max = max;
	m_min = min;
	m_max -= m_min;
	m_time /= m_totalTime;

	if (m_time < 1 / 2.75f)
		return m_max * (7.5625f * m_time * m_time) + m_min;
	else if (m_time < 2 / 2.75f)
	{
		m_time -= 1.5f / 2.75f;
		return m_max * (7.5625f * m_time * m_time + 0.75f) + m_min;
	}
	else if (m_time < 2.5f / 2.75f)
	{
		m_time -= 2.25f / 2.75f;
		return max * (7.5625f * m_time * m_time + 0.9375f) + m_min;
	}
	else
	{
		m_time -= 2.625f / 2.75f;
		return max * (7.5625f * m_time * m_time + 0.984375f) + m_min;
	}
}
float Easing::InBounce(float time, float totalTime, float max, float min)
{
	return max - OutBounce(totalTime - time, totalTime, max - min, 0) + min;
}
float Easing::InOutBounce(float time, float totalTime, float max, float min)
{
	if (time < totalTime / 2)
		return InBounce(time * 2, totalTime, max - min, max) * 0.5f + min;
	else
		return OutBounce(time * 2 - totalTime, totalTime, max - min, 0) * 0.5f + min + (max - min) * 0.5f;
}

float Easing::InExp(float time, float totalTime, float max, float min)
{
	max -= min;
	return time == 0.0 ? min : max * ::powf(2, 10 * ((time / totalTime - 1))) + min;
}
float Easing::OutExp(float time, float totalTime, float max, float min)
{
	max -= min;
	return time == totalTime ? max + min : max * (-::powf(2, -10 * time / totalTime) + 1) + min;
}
float Easing::InOutExp(float time, float totalTime, float max, float min)
{
	if (time == 0.0)
		return min;
	if (time == totalTime)
		return max;
	max -= min;
	time /= totalTime;

	if (time / 2 < 1)
		return max / 2 * ::powf(2.0f, 10.0f * (time - 1.0f)) + min;
	--time;
	return max / 2.0f* (-::powf(2.0f, -10.0f * time) + 2.0f) + min;

}
float Easing::InCirc(float time, float totalTime, float max, float min)
{
	max -= min;
	time /= totalTime;
	return -max * (::sqrtf(1.0f - time * time) - 1.0f) + min;
}
float Easing::OutCirc(float time, float totalTime, float max, float min)
{
	max -= min;
	time = time / totalTime - 1.0f;
	return max * ::sqrtf(1.0f - time * time) + min;
}
float Easing::InOutCirc(float time, float totalTime, float max, float min)
{
	max -= min;
	time /= totalTime;
	if (time / 2 < 1)
		return -max / 2.0f * (::sqrtf(1.0f - time * time) - 1.0f) + min;
	time -= 2.0f;
	return max / 2.0f * (::sqrtf(1.0f - time * time) + 1.0f) + min;
}
float Easing::InBack(float time, float totalTime, float max, float min, float s)
{
	max -= min;
	time /= totalTime;
	return max * time * time * ((s + 1.0f) * time - s) + min;
}
float Easing::OutBack(float time, float totalTime, float max, float min, float s)
{
	max -= min;
	time = time / totalTime - 1.0f;
	return max * (time * time * ((s + 1) * time * s) + 1.0f) + min;
}
float Easing::InOutBack(float time, float totalTime, float max, float min, float s)
{
	max -= min;
	s *= 1.525f;
	if (time / 2 < 1)
	{
		return max * (time * time * ((s + 1.0f) * time - s)) + min;
	}
	time -= 2.0f;
	return max / 2.0f * (time * time * ((s + 1.0f) * time + s) + 2.0f) + min;
}