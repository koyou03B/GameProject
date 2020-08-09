#pragma once
#define XAUDIO2_HELPER_FUNCTIONS
#include <xaudio2.h>

#include <queue>
#include <memory>
//*****************************************
//	FIFO(ファーストインファーストアウト)
//	std::queue
//*****************************************

namespace Source
{
	namespace SoundBuffer
	{
		class SoundBuffer
		{
		public:
			SoundBuffer(LPCWSTR szFilename);
			~SoundBuffer() = default;

			HRESULT CreateSourceVoice(IXAudio2* pXaudio2, bool loop);
			void Update();
			void Uninitlize();

			void PlayWave(IXAudio2* pXaudio2)
			{
				if (m_isPlay)
				{
					if (m_pSourceVoice.empty())
					{
						CreateSourceVoice(pXaudio2, m_isLoop);
					}
					m_pSourceVoice.front()->Start(0);

				}
			}

			inline void StopWave()
			{
				m_isPlay = !m_isPlay;
				m_pSourceVoice.front()->Stop();
			}

			inline void SetVolume(float volume)
			{
				m_volume = volume;
				m_pSourceVoice.front()->SetVolume(m_volume);
			}

			inline void SetPitch(float pitch)
			{
				m_pitch = pitch;
				m_pSourceVoice.front()->SetFrequencyRatio(m_pitch);
			}

			void SetOutputMatrix(int outChannels, float volumeLeft, float volumeRight)
			{
				int inChannels = GetInChannels();
				if (inChannels == 2)
				{
					float volumes[] = { 0.0f,volumeLeft,0.0f,volumeRight };
					m_pSourceVoice.front()->SetOutputMatrix(NULL, inChannels, outChannels, volumes);
				}
				else
				{
					float volumes[] = { volumeLeft,volumeRight };
					m_pSourceVoice.front()->SetOutputMatrix(NULL, inChannels, outChannels, volumes);
				}
			}
			inline float GetVolume() { return m_volume; }

			int GetInChannels()
			{
				XAUDIO2_VOICE_DETAILS details;
				m_pSourceVoice.front()->GetVoiceDetails(&details);
				return details.InputChannels;
			}

		private:

			struct WAVData
			{
				const WAVEFORMATEX* waveFormaTex;
				const unsigned char* startAudio;
				unsigned int audioBytes;
				unsigned int loopStart;
				unsigned int loopLength;
				const unsigned int* seek;
				unsigned int seekCount;
			};

			std::queue<IXAudio2SourceVoice*> m_pSourceVoice;
			std::unique_ptr<uint8_t[]> m_waveFiles;
			WAVData m_waveData;
			float m_volume;
			float m_pitch;
			bool m_isPlay;
			bool m_isLoop;

			HRESULT Initialize(LPCWSTR szFilename);
			HRESULT FindMediaFile(wchar_t* strDestPath, int cchDest, LPCWSTR strFilename);
			HRESULT LoadWAVAudioFromFile(const wchar_t* szFileName, std::unique_ptr<uint8_t[]>& wavData, WAVData& result);


		};
	}
}
