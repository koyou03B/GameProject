#include <assert.h>
#include "Sound.h"
#include "Vector.h"
#include "Easing.h"

namespace Source
{
	namespace Sound
	{
		Sound::Sound(const char* filename, bool isLoop)
		{
			UINT32 flags = 0;
#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
			flags |= XAUDIO2_DEBUG_ENGINE;
#endif
			HRESULT hr = XAudio2Create(&m_pXaudio2, flags);
			if (FAILED(hr))
			{
				CoUninitialize();
				assert(!"Could not Create XAudio2");
			}

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
			// To see the trace output, you need to view ETW logs for this application:
			//    Go to Control Panel, Administrative Tools, Event Viewer.
			//    View->Show Analytic and Debug Logs.
			//    Applications and Services Logs / Microsoft / Windows / XAudio2. 
			//    Right click on Microsoft Windows XAudio2 debug logging, Properties, then Enable Logging, and hit OK 
			XAUDIO2_DEBUG_CONFIGURATION debug = { 0 };
			debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
			debug.BreakMask = XAUDIO2_LOG_ERRORS;
			m_pXaudio2->SetDebugConfiguration(&debug, 0);
#endif

			// Create a mastering voice
			hr = m_pXaudio2->CreateMasteringVoice(&m_pMasteringVoice);
			if (FAILED(hr))
			{
				wprintf(L"Failed creating mastering voice: %#X\n", hr);
				if (m_pXaudio2 != nullptr)
				{
					m_pXaudio2->Release();
					m_pXaudio2 = nullptr;
				}
				CoUninitialize();
				assert(!"Could not Create MasteringVoice");
			}

			wchar_t wavNameW[256];
			mbstowcs_s(0, wavNameW, filename, strlen(filename) + 1);
			m_soundBuffer = std::make_unique<Source::SoundBuffer::SoundBuffer>(wavNameW);

			hr = m_soundBuffer->CreateSourceVoice(m_pXaudio2, isLoop);
			if (FAILED(hr))
				assert(!"Could not Create SourceVoice");
		}

		Sound::~Sound()
		{
			m_soundBuffer->Uninitlize();

			if (m_pXaudio2 != nullptr)
			{
				if (m_pMasteringVoice != nullptr)
				{
					m_pMasteringVoice->DestroyVoice();
					m_pMasteringVoice = nullptr;
				}

				m_pXaudio2->Release();
				m_pXaudio2 = nullptr;
			}
			CoUninitialize();
		}

		void Sound::Update()
		{
			m_soundBuffer->Update();
		}

		void Sound::SoundFade(const float time, const float targetVolume, const float targetTime)
		{
			float volume = Easing().GetInstance().InQuad(time, targetTime, targetVolume, m_soundBuffer->GetVolume());
			m_soundBuffer->SetVolume(volume);
		}

		void Sound::Play()
		{

			m_soundBuffer->PlayWave(m_pXaudio2);
		}

		void Sound::Stop()
		{
			m_soundBuffer->StopWave();
		}

		void Sound::SetVolume(const float volume)
		{
			float newVolume = volume;
			if (newVolume < 0.0f) newVolume = 0.0f;
			else if (2.0f < newVolume) newVolume = 2.0f;
			m_soundBuffer->SetVolume(newVolume);
		}

		void Sound::SetPitch(const float pitch)
		{
			float newPitch = pitch;
			if (pitch < 0.0f) newPitch = 0.0f;
			else if (2.0f < pitch) newPitch = 2.0f;
			m_soundBuffer->SetPitch(pitch);
		}

	}
}

