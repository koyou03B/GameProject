#include "SoundData.h"

namespace Source
{
	namespace SoundData
	{
		LoadSound loadSound[] =
		{
			// ’Ç‰Á—p     { SoundLabel::, "../Asset/Sounds/BGM/.wav" },
			{ SoundLabel::BGM_Title, "../Asset/Sounds/BGM/GAMABGM.wav", true},
			{ SoundLabel::SE_Select, "../Asset/Sounds/SE/Voice.wav", true}

		};

		void SoundData::Load()
		{
			for (int i = 0; i < static_cast<int>(SoundLabel::END); i++)
			{
				m_sound[i] = std::make_unique<Source::Sound::Sound>(loadSound[i].filename, loadSound[i].isLoop);
			}
		}

		void SoundData::SoundPlay(SoundLabel soundNum)
		{
			if (soundNum < SoundLabel::END)
				m_sound[static_cast<int>(soundNum)]->Play();
		}

		void SoundData::Update()
		{
			for (auto& it : m_sound)
			{
				it->Update();
			}
		}

		void SoundData::Release()
		{
			for (auto& it : m_sound)
			{
				it->~Sound();
			}
		}

		void SoundData::SoundStop(SoundLabel soundNum)
		{
			if (soundNum < SoundLabel::END)
				m_sound[static_cast<int>(soundNum)]->Stop();
		}

		void SoundData::SoundVolume(SoundLabel soundNum, const float volume)
		{
			if (soundNum < SoundLabel::END)
				m_sound[static_cast<int>(soundNum)]->SetVolume(volume);
		}

		void SoundData::SoundPitch(SoundLabel soundNum, const float volume)
		{
			if (soundNum < SoundLabel::END)
				m_sound[static_cast<int>(soundNum)]->SetPitch(volume);
		}

		void SoundData::SoundOutputMatrix(SoundLabel soundNum, float volumeLeft, float volumeRight)
		{
			if (soundNum < SoundLabel::END)
				m_sound[static_cast<int>(soundNum)]->SetOutputMatrix(volumeLeft, volumeRight);
		}

		bool SoundData::SoundFade(SoundLabel soundNum, const float targetVolume, const float targetTime, const float time)
		{
			if (soundNum < SoundLabel::END)
			{
				if (time < targetTime)
				{
					m_sound[static_cast<int>(soundNum)]->SoundFade(time, targetVolume, targetTime);
					return true;
				}
			}
			return false;
		}
	}
}
