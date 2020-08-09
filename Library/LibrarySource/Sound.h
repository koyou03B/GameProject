#pragma once
#include "SoundBuffer.h"

namespace Source
{
	namespace Sound
	{
		class Sound
		{
		public:
			Sound(const char* name, bool isLoop);
			~Sound();

			void Update();
			void SoundFade(const float time, const float trgetVolume, const float targetTime);
			void Play();
			void Stop();
			void SetVolume(const float volume);
			void SetPitch(const float pitch);

			void SetOutputMatrix(float volumeLeft, float volumeRight)
			{
				int outChannels = GetOutChannels();
				m_soundBuffer->SetOutputMatrix(outChannels, volumeLeft, volumeRight);
			}

			int GetInChannel()
			{
				return m_soundBuffer->GetInChannels();
			}

			int GetOutChannels()
			{
				XAUDIO2_VOICE_DETAILS details;
				m_pMasteringVoice->GetVoiceDetails(&details);
				return details.InputChannels;
			}
		private:
			IXAudio2* m_pXaudio2 = nullptr;
			IXAudio2MasteringVoice* m_pMasteringVoice = nullptr;

			std::unique_ptr<Source::SoundBuffer::SoundBuffer> m_soundBuffer;

			const char* m_pWavName;
		};
	}
}
