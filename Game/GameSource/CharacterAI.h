#pragma once

#include <memory>
#include <d3d11.h>
#include "State.h"
#include "AIParameter.h"
#include "CharacterParameter.h"
#include ".\LibrarySource\Input.h"
#include ".\LibrarySource\Camera.h"
#include ".\LibrarySource\SkinnedMesh.h"

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

	virtual void Release()
	{
		if (m_model)
		{
			if (m_model.unique())
			{
				m_model.reset();
			}
		}
	};

	virtual void ImGui(ID3D11Device* device) = 0;

	inline int GetID() { return m_id; }
	inline std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& GetModel() { return m_model; }
	inline CharacterParameter::WorldTransform& GetWorldTransform() { return m_transformParm; }
	inline CharacterParameter::Status& GetStatus() { return m_statusParm; }
	inline CharacterParameter::Move& GetMove() { return m_moveParm; }
	inline CharacterParameter::Change& GetChangeComand() { return m_changeParm; }
	inline CharacterParameter::MessageFlg& GetMessageFlg() { return m_messageParm; }
	inline CharacterParameter::Camera& GetCamera() { return m_cameraParm; }
	inline std::vector<CharacterParameter::Collision>& GetCollision() { return m_collision; }
	inline AIParameter::Emotion& GetEmotion() { return m_emotionParm; }
	inline AIParameter::JudgeElement& GetJudgeElement() { return m_judgeElementPram; }
	inline AIParameter::StandardValue& GetStandardValue() { return m_standardValuePram; }
	inline void SetID(int id) { m_id = id; }
	inline void SetState(State* state) { m_state = state; }
	inline void SetCharacter(std::shared_ptr<Source::SkinnedMesh::SkinnedMesh> model) { m_model = model; }

protected:
	int m_id = -1;
	std::shared_ptr<Source::SkinnedMesh::SkinnedMesh> m_model;

	CharacterParameter::WorldTransform			m_transformParm;
	CharacterParameter::Status					m_statusParm;
	CharacterParameter::Move					m_moveParm;
	CharacterParameter::Change					m_changeParm;
	CharacterParameter::MessageFlg				m_messageParm;
	CharacterParameter::Camera					m_cameraParm;
	AIParameter::Emotion						m_emotionParm;
	AIParameter::JudgeElement					m_judgeElementPram;
	AIParameter::StandardValue					m_standardValuePram;
	std::vector<CharacterParameter::Collision>	m_collision;
	State* m_state;

};