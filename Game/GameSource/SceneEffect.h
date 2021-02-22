#pragma once
#include <memory>
#include <vector>
#include <d3d11.h>

#include ".\LibrarySource\Fog.h"
#include ".\LibrarySource\ScreenFilter.h"
#include ".\LibrarySource\Vignette.h"
#include ".\LibrarySource\FrameBuffer.h"

enum SceneEffectType
{
	SCREEN_FILTER,
	VIGNETTE,
	FOG,
	TYPE_END
};

class SceneEffect
{
public:
	SceneEffect() = default;
	~SceneEffect() = default;

	void ChoiceSceneEffect(ID3D11Device* device,const SceneEffectType type);
	bool UpdateScreenFilter(const float value, const float targetValue);
	void UpdateVignette(const float targetTime, const float targetDarkness,
		const float darknessValue, float& elapsedTime);
	
	void ActivateEffect(ID3D11DeviceContext* immediateContext, const SceneEffectType type);
	void DeactivateEffect(ID3D11DeviceContext* immediateContext,const SceneEffectType type);

	void ImGui();
	inline void StartVignette() { m_vignette.isStartEffect = true; }

private:
	struct FogEffect
	{
		bool isUpdateEffect;
		std::unique_ptr<Source::Fog::Fog> fog;

		void InitOfFog(ID3D11Device* device);
	} m_fog;

	struct ScreenFilterEffect
	{
		std::unique_ptr<Source::ScreenFilter::ScreenFilter> screenFilter;

		void InitOfScreenFilter(ID3D11Device* device);
		bool Update(const float value, const float targetValue);
	} m_screenFilter;

	struct VignetteEffect
	{
		float vignetteTimer;
		bool isStartEffect;
		std::unique_ptr<Source::Vignette::Vignette> vignette;

		void InitOfVignette(ID3D11Device* device);
		void Update(const float targetTime, const float targetDarkness, 
			const float darknessValue,float& elapsedTime);
	} m_vignette;

	std::unique_ptr<Source::FrameBuffer::FrameBuffer> m_frameBuffer[SceneEffectType::TYPE_END];
	std::vector<SceneEffectType> m_activateEffect;
};