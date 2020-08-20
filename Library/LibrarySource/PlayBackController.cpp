#include <math.h>
#include "PlayBackController.h"
#include "Function.h"

namespace Source
{
	namespace Blend
	{
		void PlayBackController::UpdateRatio(float& duration, float& elapsedTime)
		{
			float newTime = m_timeRatio;

			if (m_isPlay)
				newTime = m_timeRatio + elapsedTime * m_playBackSpeed / duration;

			SetTimeratio(newTime);

		}

		void PlayBackController::Reset()
		{
			m_previousTimeRatio = m_timeRatio = 0.0f;
			m_playBackSpeed = 1.0f;
			m_isPlay = true;
			m_isLoop = true;
		}

		void PlayBackController::SetTimeratio(float time)
		{
			m_previousTimeRatio = m_timeRatio;

			if (m_isLoop)
				m_timeRatio = time - floorf(time);
			else
				m_timeRatio = Source::Math::Clamp(0.0f, time, 1.0f);

		}

	}
}
