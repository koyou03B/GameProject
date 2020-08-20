#pragma once

#include <memory>
#include <d3d11.h>
#include "State.h"
#include "CharacterParameter.h"
#include ".\LibrarySource\SkinnedMesh.h"
#include ".\LibrarySource\Camera.h"
#include ".\LibrarySource\Input.h"
#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif

class CharacterAI
{
public:
	CharacterAI() = default;
	~CharacterAI() = default;

	virtual void Init() = 0;

	virtual void Update(float& elapsedTime) = 0;

	virtual void Render(ID3D11DeviceContext* immediateContext) = 0;

	virtual void ImGui(ID3D11Device* device) = 0;

	inline void SetState(State* state) { m_state = state; }
	inline int GetID() { return m_id; }
	inline std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& GetModel()  { return m_model; }
	inline CharacterParameter::WorldTransform& GetWorldTransform()		  { return m_transformParm; }
	inline CharacterParameter::Change& GetChangeComand()				  { return m_changeComand; }
	inline CharacterParameter::Status& GetStatus()						  { return m_status; }
	inline CharacterParameter::MessageFlg& GetMessageFlg()				  { return m_message; }
	inline CharacterParameter::Camera& GetCamera()						  { return m_cameraParm; }

	void SetID(int id) { m_id = id; }
	inline void SetCharacter(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh> model){ m_model = model;}

protected:
	int m_id = -1;
	std::shared_ptr<Source::SkinnedMesh::SkinnedMesh> m_model;

	CharacterParameter::WorldTransform m_transformParm;
	CharacterParameter::Status		   m_status;
	CharacterParameter::Change		   m_changeComand;
	CharacterParameter::MessageFlg     m_message;
	CharacterParameter::Camera		   m_cameraParm;
	State* m_state;

};