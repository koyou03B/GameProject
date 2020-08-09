#pragma once

namespace Source
{
	namespace Blend
	{
		class PlayBackController
		{
		public:
			PlayBackController() : m_timeRatio(0.0f), m_previousTimeRatio(0.0f),
				m_playBackSpeed(1.0f), m_isPlay(true), m_isLoop(true) {}
			~PlayBackController() = default;

			void UpdateRatio(float& duration, float& elapsedTime);

			void Reset();

			inline float GetPreviousTimeRatio() { return m_previousTimeRatio; }
			inline float GetTimeRatio() { return m_timeRatio; }
			inline float GetPlayBackSpeed() { return m_playBackSpeed; }
			inline bool  GetIsLoop() { return m_isLoop; }

			void SetTimeratio(float time);
			void SetPlayBackSpeed(float speed) { m_playBackSpeed = speed; }
			void SetIsLoop(bool loop) { m_isLoop = loop; }

		private:
			float m_timeRatio;
			float m_previousTimeRatio;
			float m_playBackSpeed;
			bool  m_isPlay;
			bool  m_isLoop;

		};

	}
}
