#pragma once
#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <DirectXMath.h>
#include <vector>
#include "HighResolutionTimer.h"
#include "SpriteBatch.h"
#include "Input.h"
#include "Function.h"
#include "SpriteData.h"
#include "SoundData.h"
#include "..\Game\\GameSource\Scene.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

class Framework
{
public:
	//DepthStencilState
	enum { DS_FALSE, DS_TRUE, DS_MAX };
	//RasterizerState
	enum { RS_CULL_BACK, RS_CULL_BACK_WIRE, RS_CULL_FRONT, RS_CULL_FRONT_WIRE, RS_CULL_NONE, RS_CLOCK_TRUE, RS_DEPTH_FALSE, RS_MAX };
	//BlendState
	enum { BS_NONE, BS_ALPHA, BS_ADD, BS_SUBTRACT, BS_REPLACE, BS_MULTIPLY, BS_LIGHTEN, BS_DARKEN, BS_SCREEN, BS_MAX };
	//SamplerState
	enum { SS_WRAP, SS_MIRROR, SS_BORDER, SS_MAX };
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	m_depthStencilState[DS_MAX];
	bool CreateDepthStencil();

	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	m_rasterizerState[RS_MAX];
	bool CreateRasterizerState();

	Microsoft::WRL::ComPtr<ID3D11BlendState>		m_blendState[BS_MAX];
	bool CreateBlendState();

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		m_samplerState[SS_MAX];
	bool CreateSamplerState();

public:
	const float SCREEN_WIDTH = 1920;
	const float SCREEN_HEIGHT = 1080;

	Framework() = default;
	Framework(HWND hwnd, HINSTANCE hinstance) :m_hwnd(hwnd), m_hinstance(hinstance)
	{

	}
	~Framework()
	{
		Source::Shader::ReleaseAllCachedVertexShaders();
		Source::Shader::ReleaseAllCachedPixelShaders();
		Source::Texture::ReleaseAllCachedTextures();
		m_swapChain->SetFullscreenState(FALSE, nullptr);
		
	}
	int Run()
	{
		MSG msg = {};
		if (!Initialize()) return 0;

#ifdef _DEBUG
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(m_hwnd);
		ImGui_ImplDX11_Init(m_device.Get(), m_deviceContext.Get());

		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
#endif

		m_timer.Reset();

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				m_timer.Tick();
				CalculateFrameStats();
				float elapsedTime = m_timer.TimeInterval();
				const float limitElapsedTime = 1.0f / 5;
				elapsedTime = elapsedTime > limitElapsedTime ? limitElapsedTime : elapsedTime;

				Update(elapsedTime);
				Render(elapsedTime);
			}
		}

#ifdef _DEBUG
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif

		return static_cast<int>(msg.wParam);
	}

	LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
#ifdef _DEBUG
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		{
			return 1;
		}
#endif
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KILLFOCUS:
			DINPUT.ReleaseDInputMouse();
			DINPUT.ReleaseDInputKeyboard();
			DINPUT.ReleaseDInput();		// DirectInputオブジェクトの開放

			break;
		case WM_SETFOCUS:
			DINPUT.InitDInput(m_hinstance, hwnd);
			DINPUT.InitDInputMouse(hwnd);
			DINPUT.InitDInputKeyboard(hwnd);
			break;
		case WM_ACTIVATEAPP:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_ENTERSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			m_timer.Stop();
			break;
		case WM_EXITSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
			m_timer.Start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

	static Framework& GetInstance(HWND hwnd = nullptr, HINSTANCE ins = nullptr)
	{
		static Framework instance(hwnd, ins);
		return instance;
	}
	static ID3D11Device* GetDevice() { return Framework::GetInstance().m_device.Get(); };
	static ID3D11DeviceContext* GetContext() { return Framework::GetInstance().m_deviceContext.Get(); };
	static ID3D11DepthStencilState* GetDephtStencilState(int state) { return Framework::GetInstance().m_depthStencilState[state].Get(); }
	static ID3D11RasterizerState* GetRasterizerState(int state) { return Framework::GetInstance().m_rasterizerState[state].Get(); }
	static ID3D11SamplerState** GetSamplerState(int state) { return Framework::GetInstance().m_samplerState[state].GetAddressOf(); }
	static ID3D11BlendState* GetBlendState(int state) { return Framework::GetInstance().m_blendState[state].Get(); }

	std::shared_ptr<Source::Sprite::SpriteBatch> GetTexture(Source::SpriteData::TextureLabel texNum) { return Framework::GetInstance().m_texture->GetSprite(texNum); }

	private:
		const HWND m_hwnd;
		HINSTANCE m_hinstance;

		Microsoft::WRL::ComPtr<ID3D11Device>			m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>		m_deviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain>			m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	m_renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_depthStencilTexture;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_depthStencilView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_backBuffer;

		D3D11_VIEWPORT m_viewPort = {};

		float m_volume[2] = {};
		float m_color[4] = { 0.3f, 0.3f, 0.3f, 1.f };

		bool Initialize();
		void Update(float elapsedTime/*Elapsed seconds from last frame*/);
		void Render(float elapsedTime/*Elapsed seconds from last frame*/);

		bool InitDevice();
		void SetViewPort(int width, int height);
		bool InitializeRenderTarget();


		HighResolutionTimer m_timer;
		void CalculateFrameStats()
		{
			// Code computes the average frames per second, and also the 
			// average time it takes to render one frame.  These stats 
			// are appended to the window caption bar.
			static int frames = 0;
			static float timeTlapsed = 0.0f;

			frames++;

			// Compute averages over one second period.
			if ((m_timer.TimeStamp() - timeTlapsed) >= 1.0f)
			{
				float fps = static_cast<float>(frames); // fps = frameCnt / 1
				float mspf = 1000.0f / fps;
				std::ostringstream outs;
				outs.precision(6);
				outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
				SetWindowTextA(m_hwnd, outs.str().c_str());

				// Reset for next average.
				frames = 0;
				timeTlapsed += 1.0f;
			}
		}
		std::shared_ptr<Source::SpriteData::SpriteData> m_texture;

};



