#pragma once
#include <memory>
#include "Sound.h"

namespace Source
{
	namespace SoundData
	{
		enum class SoundLabel
		{
			BGM_Title,
			SE_Select,

			END
		};

		struct LoadSound
		{
			SoundLabel soundNum;
			const char* filename;
			bool isLoop;
		};

		class SoundData
		{

		public:
			SoundData() {}
			~SoundData() {}

			void Load();
			void Update();
			void Release();
			void SoundPlay(SoundLabel soundNum);
			void SoundStop(SoundLabel soundNum);
			void SoundVolume(SoundLabel soundNum, const float volume);
			void SoundPitch(SoundLabel soundNum, const float volume);
			void SoundOutputMatrix(SoundLabel soundNum, float volumeLeft, float volumeRight);

			int GetSoundInChannels(SoundLabel soundNum) { return m_sound[static_cast<int>(soundNum)]->GetInChannel(); }
			int GetSoundOutChannels(SoundLabel soundNum) { return m_sound[static_cast<int>(soundNum)]->GetOutChannels(); }

			bool SoundFade(SoundLabel soundNum, const float targetVolume, const float targetTime, const float time);

			inline static SoundData& GetInstance()
			{
				static SoundData instance;
				return instance;
			}

		private:
			std::unique_ptr<Source::Sound::Sound> m_sound[static_cast<int>(SoundLabel::END)];

		};
	}
}

#define SoundTrack Source::SoundData::SoundData::GetInstance()