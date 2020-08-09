#pragma once
#include <memory>
#include <d3d11.h>
#include ".\LibrarySource\SkinnedMesh.h"

class PlayCharacter
{
public:
	PlayCharacter() = default;
	~PlayCharacter() = default;

	virtual void Init()
	{
		Init();
	}

	virtual void Update(float& elapsedTime)
	{
		Update(elapsedTime);
	}

	virtual void Render(ID3D11DeviceContext* immediateContext)
	{
		Render(immediateContext);
	}

	virtual void ImGui(ID3D11Device* device)
	{
		ImGui(device);
	}

	std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& GetModel()
	{
		return m_model;
	}

	inline void SetCharacter(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh> model)
	{
		m_model = model;
	}

protected:
	std::shared_ptr<Source::SkinnedMesh::SkinnedMesh> m_model;
};