#include "Scene.h"
#include "SceneTitle.h"
#include "SceneTutorial.h"
#include "SceneGame.h"
#include "SceneOver.h"
#include "SceneClear.h"

void SceneManager::ChangeScene(Scene::SceneLabel scene)
{
	if (m_activeScene)
	{
		Uninitialize();
		delete m_activeScene;
		m_activeScene = nullptr;
		m_isStart = true;
	}

	switch (scene)
	{
	case Scene::TITLE:
		m_activeScene = new Title;
		break;
	case Scene::TUTORIAL:
		m_activeScene = new Tutorial;
		break;
	case Scene::GAME:
		m_activeScene = new Game;
		break;
	case Scene::OVER:
		m_activeScene = new Over;
		break;
	case Scene::CLEAR:
		m_activeScene = new Clear;
		break;
	default:
		break;
	}

}

void SceneManager::Initialize(ID3D11Device* device)
{
	if (!m_activeScene || !m_isStart) return;

	m_activeScene->Initialize(device);
	m_isStart = false;
}

void SceneManager::Update(float& elapsedTime)
{
	if (!m_activeScene) return;

	//if (_activeScene->IsLoading())
	//{
	//}
	m_activeScene->Update(elapsedTime);


}

void SceneManager::Render(ID3D11DeviceContext* immediateContext, float elapsedTime)
{
	if (!m_activeScene) return;

	//if (_activeScene->IsLoading())
	//{

	//}
	//else
	//{

	//}
	m_activeScene->Render(immediateContext, elapsedTime);

	if (m_isDebugMode)
		m_activeScene->ImGui();
}

void SceneManager::Uninitialize()
{
	m_activeScene->Uninitialize();
}
