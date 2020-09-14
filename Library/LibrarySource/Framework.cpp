#include"Framework.h"
#include "..\Game\GameSource\Scene.h"
#include"misc.h"


bool Framework::Initialize()
{
	InitDevice();
	InitializeRenderTarget();
	CreateRasterizerState();
	CreateBlendState();
	CreateSamplerState();

	SoundTrack.Load();
	m_volume[0] = 0.0f;
	m_volume[1] = 1.0f;

	KEYBOARD.InitDInput(m_hinstance, m_hwnd);
	KEYBOARD.InitDInputKeyboard(m_hwnd);
	MOUSE.InitDInput(m_hinstance, m_hwnd);
	MOUSE.InitDInputMouse(m_hwnd);
	PAD.SetUpPads();
	ActivateScene.ChangeScene(Scene::SceneLabel::TITLE);
	ActivateScene.Initialize(m_device.Get());

	//SoundTrack.SoundPlay(SoundLabel::SE_Select);

	return true;

}

//------------------------------------------------
//	Initialization of the Device
//------------------------------------------------
bool Framework::InitDevice()
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<IDXGIFactory> hpDXGIFactory;
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(hpDXGIFactory.GetAddressOf()));
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	IDXGIAdapter* dxgiAdapter;
	std::vector<IDXGIAdapter*> dxgiAdapters;
	for (UINT i = 0; hpDXGIFactory->EnumAdapters(i, &dxgiAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		dxgiAdapters.push_back(dxgiAdapter);
	}
	dxgiAdapter = dxgiAdapters[0];
	DXGI_ADAPTER_DESC dxgiAdapterDesc;
	hr = dxgiAdapter->GetDesc(&dxgiAdapterDesc);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	UINT createDeviceFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_UNKNOWN,
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	D3D_FEATURE_LEVEL featureLevel;

	for (D3D_DRIVER_TYPE driverType : driverTypes)
	{
		hr = D3D11CreateDevice(dxgiAdapter, driverType, NULL, createDeviceFlags, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
			m_device.GetAddressOf(), &featureLevel, m_deviceContext.GetAddressOf());

		if (SUCCEEDED(hr)) break;

		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		_ASSERT_EXPR(!(featureLevel < D3D_FEATURE_LEVEL_11_0), L"Direct3D Feature Level 11 unsupported");
	}

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = (UINT)SCREEN_WIDTH;
	swapChainDesc.BufferDesc.Height = (UINT)SCREEN_HEIGHT;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = m_hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	hr = hpDXGIFactory->CreateSwapChain(m_device.Get(), &swapChainDesc, m_swapChain.GetAddressOf());

	if (FAILED(hr))
		assert(!"Could not Create  SwapChain");


	return true;

}

//------------------------------------------------
//	Initialization of the RenderTarget
//------------------------------------------------
bool Framework::InitializeRenderTarget()
{
	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)m_backBuffer.GetAddressOf());

	if (FAILED(hr))
		assert(!"Could not Get Buffer");


	hr = m_device->CreateRenderTargetView(m_backBuffer.Get(), NULL, m_renderTargetView.GetAddressOf());

	if (FAILED(hr))
		assert(!"Could not Create RenderTargetView");


	CreateDepthStencil();

	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

	SetViewPort((int)SCREEN_WIDTH, (int)SCREEN_HEIGHT);

	return true;
}

//------------------------------------------------
//	Create of the DepthStencil
//------------------------------------------------
bool Framework::CreateDepthStencil()
{

	D3D11_TEXTURE2D_DESC texture2dDesc;
	ZeroMemory(&texture2dDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texture2dDesc.Width = (UINT)SCREEN_WIDTH;
	texture2dDesc.Height = (UINT)SCREEN_HEIGHT;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;

	HRESULT hr = m_device->CreateTexture2D(&texture2dDesc, NULL, &m_depthStencilTexture);

	if (FAILED(hr))
		assert(!"Could not Create Texture2D");


	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = m_device->CreateDepthStencilView(m_depthStencilTexture.Get(), &depthStencilViewDesc, m_depthStencilView.GetAddressOf());

	if (FAILED(hr))
		assert(!"Could not Create DepthStencilView");

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState[DS_FALSE].GetAddressOf());

	if (FAILED(hr))
		assert(!"Could not Create DepthStencilState");

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState[DS_TRUE].GetAddressOf());

	if (FAILED(hr))
		assert(!"Could not Create DepthStencilState");

	return true;
}

//------------------------------------------------
//	Create of the Rasterizer
//------------------------------------------------
bool Framework::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerDesc;
	int rasterizeSize = static_cast<int>(RS_MAX);
	for (int state = 0; state < rasterizeSize; state++)
	{
		switch (state) {
		case RS_CULL_BACK:
			ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.FrontCounterClockwise = FALSE;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.DepthBiasClamp = 0;
			rasterizerDesc.SlopeScaledDepthBias = 0;
			rasterizerDesc.DepthClipEnable = FALSE;
			rasterizerDesc.ScissorEnable = FALSE;
			rasterizerDesc.MultisampleEnable = FALSE;
			rasterizerDesc.AntialiasedLineEnable = FALSE;
			break;
		case RS_CULL_BACK_WIRE:
			ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
			rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.FrontCounterClockwise = FALSE;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.DepthBiasClamp = 0;
			rasterizerDesc.SlopeScaledDepthBias = 0;
			rasterizerDesc.DepthClipEnable = TRUE;
			rasterizerDesc.ScissorEnable = FALSE;
			rasterizerDesc.MultisampleEnable = FALSE;
			rasterizerDesc.AntialiasedLineEnable = FALSE;
			break;
		case RS_CULL_FRONT:
			ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_FRONT;
			rasterizerDesc.FrontCounterClockwise = FALSE;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.DepthBiasClamp = 0;
			rasterizerDesc.SlopeScaledDepthBias = 0;
			rasterizerDesc.DepthClipEnable = TRUE;
			rasterizerDesc.ScissorEnable = FALSE;
			rasterizerDesc.MultisampleEnable = FALSE;
			rasterizerDesc.AntialiasedLineEnable = FALSE;
			break;
		case RS_CULL_FRONT_WIRE:
			ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
			rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
			rasterizerDesc.CullMode = D3D11_CULL_FRONT;
			rasterizerDesc.FrontCounterClockwise = FALSE;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.DepthBiasClamp = 0;
			rasterizerDesc.SlopeScaledDepthBias = 0;
			rasterizerDesc.DepthClipEnable = TRUE;
			rasterizerDesc.ScissorEnable = FALSE;
			rasterizerDesc.MultisampleEnable = FALSE;
			rasterizerDesc.AntialiasedLineEnable = FALSE;
			break;
		case RS_CULL_NONE:
			ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			rasterizerDesc.FrontCounterClockwise = FALSE;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.DepthBiasClamp = 0;
			rasterizerDesc.SlopeScaledDepthBias = 0;
			rasterizerDesc.DepthClipEnable = TRUE;
			rasterizerDesc.ScissorEnable = FALSE;
			rasterizerDesc.MultisampleEnable = FALSE;
			rasterizerDesc.AntialiasedLineEnable = FALSE;
			break;
		case RS_CLOCK_TRUE:
			ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.FrontCounterClockwise = TRUE;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.DepthBiasClamp = 0.0f;
			rasterizerDesc.SlopeScaledDepthBias = 0.0f;
			rasterizerDesc.DepthClipEnable = TRUE;
			rasterizerDesc.MultisampleEnable = FALSE;
			rasterizerDesc.ScissorEnable = FALSE;
			rasterizerDesc.AntialiasedLineEnable = FALSE;
			break;
		case RS_DEPTH_FALSE:
			ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.FrontCounterClockwise = FALSE;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.DepthBiasClamp = 0.0f;
			rasterizerDesc.SlopeScaledDepthBias = 0.0f;
			rasterizerDesc.DepthClipEnable = FALSE;
			rasterizerDesc.MultisampleEnable = FALSE;
			rasterizerDesc.ScissorEnable = FALSE;
			rasterizerDesc.AntialiasedLineEnable = FALSE;
			break;
		}
		HRESULT hr = m_device->CreateRasterizerState(&rasterizerDesc, m_rasterizerState[state].GetAddressOf());

		if (FAILED(hr))
			assert(!"Could not Create RasterizerState");

	}

	return true;

}

//------------------------------------------------
//	Create of the Sampler
//------------------------------------------------
bool Framework::CreateSamplerState()
{
	D3D11_SAMPLER_DESC samplerDesc;
	int samplerSize = static_cast<int>(SS_MAX);
	for (int state = 0; state < samplerSize; state++)
	{
		switch (state)
		{
		case SS_WRAP:
			ZeroMemory(&samplerDesc, sizeof(samplerDesc));
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerDesc.MinLOD = -FLT_MAX;
			samplerDesc.MaxLOD = FLT_MAX;
			samplerDesc.BorderColor[0] = 1.0f;
			samplerDesc.BorderColor[1] = 1.0f;
			samplerDesc.BorderColor[2] = 1.0f;
			samplerDesc.BorderColor[3] = 1.0f;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		case SS_MIRROR:
			ZeroMemory(&samplerDesc, sizeof(samplerDesc));
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerDesc.MinLOD = -FLT_MAX;
			samplerDesc.MaxLOD = FLT_MAX;
			samplerDesc.BorderColor[0] = 1.0f;
			samplerDesc.BorderColor[1] = 1.0f;
			samplerDesc.BorderColor[2] = 1.0f;
			samplerDesc.BorderColor[3] = 1.0f;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		case SS_BORDER:
			ZeroMemory(&samplerDesc, sizeof(samplerDesc));
			samplerDesc.MipLODBias = 0;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;//
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.BorderColor[0] = 1.0f;
			samplerDesc.BorderColor[1] = 1.0f;
			samplerDesc.BorderColor[2] = 1.0f;
			samplerDesc.BorderColor[3] = 1.0f;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		}
		HRESULT hr = m_device->CreateSamplerState(&samplerDesc, m_samplerState[state].GetAddressOf());

		if (FAILED(hr))
			assert(!"Could not Create SamplerState");
	}

	return true;
}

//------------------------------------------------
//	Create of the Blend
//------------------------------------------------
bool Framework::CreateBlendState()
{
	D3D11_BLEND_DESC blendDesc;
	int blendSize = static_cast<int>(BS_MAX);
	for (int state = 0; state < blendSize; state++)
	{
		switch (state) {
		case BS_NONE:
			ZeroMemory(&blendDesc, sizeof(blendDesc));
			blendDesc.IndependentBlendEnable = false;
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.RenderTarget[0].BlendEnable = false;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS_ALPHA:
			ZeroMemory(&blendDesc, sizeof(blendDesc));
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.IndependentBlendEnable = false;
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS_ADD:
			ZeroMemory(&blendDesc, sizeof(blendDesc));
			blendDesc.IndependentBlendEnable = false;
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS_SUBTRACT:
			ZeroMemory(&blendDesc, sizeof(blendDesc));
			blendDesc.IndependentBlendEnable = false;
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS_REPLACE:
			ZeroMemory(&blendDesc, sizeof(blendDesc));
			blendDesc.IndependentBlendEnable = false;
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;
		case BS_MULTIPLY:
			ZeroMemory(&blendDesc, sizeof(blendDesc));
			blendDesc.IndependentBlendEnable = false;
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;
		case BS_LIGHTEN:
			ZeroMemory(&blendDesc, sizeof(blendDesc));
			blendDesc.IndependentBlendEnable = false;
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		case BS_DARKEN:
			ZeroMemory(&blendDesc, sizeof(blendDesc));
			blendDesc.IndependentBlendEnable = false;
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;
		case BS_SCREEN:
			ZeroMemory(&blendDesc, sizeof(blendDesc));
			blendDesc.IndependentBlendEnable = false;
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			break;

		}

		HRESULT hr = m_device->CreateBlendState(&blendDesc, m_blendState[state].GetAddressOf());

		if (FAILED(hr))
			assert(!"Could not Create BlendState");

	}
	return true;
}

//------------------------------------------------
//	Viewport Settings
//------------------------------------------------
void Framework::SetViewPort(int width, int height)
{
	m_viewPort.Width = (FLOAT)width;
	m_viewPort.Height = (FLOAT)height;
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;
	m_viewPort.TopLeftX = 0;
	m_viewPort.TopLeftY = 0;
	m_deviceContext->RSSetViewports(1, &m_viewPort);

}

void Framework::Update(float elapsedTime/*Elapsed seconds from last frame*/)
{
#ifdef _DEBUG
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif

	SoundTrack.Update();
	KEYBOARD.GetKeyStateAll();
	MOUSE.MouseState(m_hwnd);
	PAD.Update();

	//SoundTrack.SoundPlay(SoundData::SoundLabel::BGM_Title);

	//if (KEYBOARD._keys[DIK_Q] == 1)
	//{
	//}
	//SoundTrack.SoundOutputMatrix(SoundLabel::SE_Select, m_volume[0], m_volume[1]);

#if 0
	//static float time = 0; ++time;
	//static bool isOut = false;
	//if (!SoundTrack.SoundFade(SoundLabel::BGM_Title, 1.0f, 600.0f, time)&& !isOut)
	//{
	//	if (KEYBOARD._keys[DIK_Q] == 1)
	//		SoundTrack.SoundStop(SoundLabel::BGM_Title);
	//	if (KEYBOARD._keys[DIK_W])
	//		SoundTrack.SoundVolume(SoundLabel::BGM_Title, 0.0f);
	//	if (KEYBOARD._keys[DIK_R])
	//		SoundTrack.SoundVolume(SoundLabel::BGM_Title, 1.0f);
	//	if (KEYBOARD._keys[DIK_E] == 1)
	//		SoundTrack.SoundPitch(SoundLabel::BGM_Title, 2.0f);
	//	if (KEYBOARD._keys[DIK_F] == 1)
	//	{
	//		isOut = true;
	//		time = 0.0f;
	//	}
	//	
	//}
	//else if (isOut)
	//{
	//	if (!SoundTrack.SoundFade(SoundLabel::BGM_Title, 0.0f, 600.0f, time))
	//	{
	//		isOut = false;
	//		time = 0;
	//		SoundTrack.SoundVolume(SoundLabel::BGM_Title, 0.1f);
	//	}
	//}

	ImGui::Begin("Framework");

	static int i = 0;
	static float j = 0;
	static float j2 = 0;
	auto input = PAD.GetPad(0);
	if (input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_UP))
		i = input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_UP);
	
	if (input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_DOWN))
		i = input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_DOWN);
	
	if (input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_RIGHT))
		i = input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_RIGHT);
	
	if (input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_LEFT))
		i = input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_LEFT);
	
	if (input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_X))
		i = input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_X);
	
	if (input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_A))
		i = input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_A);
	
	if (input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_B))
		i = input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_B);
	
	if (input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_Y))
		i = input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_Y);
	
	if (input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_LSHOULDER))
		i = input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_LSHOULDER);

	if (input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_RSHOULDER))
		i = input.GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_RSHOULDER);


//	j = JOYPAD.GetStickLxValue();
//	j = JOYPAD.GetStickLyValue();
	j = input.GetStickRxValue();
//	j = JOYPAD.GetStickRyValue();
	j2 = input.StickVectorL().x;
	ImGui::Text("%d", i);
	ImGui::Text("%f", j);
	ImGui::Text("%f", j2);
	ImGui::End();
#endif
//	ImGui::Begin("INPUT Try");
//
//	static float i = 0.0f;
//	if (MOUSE.PressedState(MouseLabel::LEFT_BUTTON))
//		i += 1.0f;
//	if (MOUSE.PressedState(MouseLabel::RIGHT_BUTTON))
//		i -= 1.0f;
//	float rad = ((i + 90.f) / 2.0f) * (M_PI / 180.0f);
//
//	m_volume[0] = cosf(rad); // 左ボリューム
//	m_volume[1] = sinf(rad);
//
//	if (m_volume[0] < 0) m_volume[0] *= -1;
//	if (m_volume[1] < 0) m_volume[1] *= -1;
//
//	ImGui::Text("i = %f", i);
//	ImGui::Text("rad = %f", rad);
//	ImGui::Text("m_volume[0] = %f", m_volume[0]);
//	ImGui::Text("m_volume[1] = %f", m_volume[1]);
//	ImGui::End();
//	SOUND.Stop(SoundLabel::BGM_Title);

	ActivateScene.Initialize(m_device.Get());
	ActivateScene.Update(elapsedTime);

}


void Framework::Render(float elapsedTime/*Elapsed seconds from last frame*/)
{


	m_deviceContext->RSSetViewports(1, &m_viewPort);
	//FRONTバッファの色をリセット
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), m_color);
	//FRONTバッファの深度をリセット
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	//深度とカラーをBACKバッファからFRONTバッファにセット
	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());


	ActivateScene.Render(m_deviceContext.Get(),elapsedTime);

#ifdef _DEBUG
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	m_swapChain->Present(0, 0);
}