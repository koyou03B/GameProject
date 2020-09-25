#pragma once
#include <d3d11.h>
#include <thread>
#include <future>
#include <chrono>
#include <memory>
using namespace std::chrono_literals;

class Scene
{
public:
    enum SceneLabel
    {
        TITLE = 0,
        TUTORIAL,
        GAME,
        OVER,
        CLEAR,
    };

public:
    std::future<void> future;


public:
    Scene() = default;
    virtual ~Scene() = default;

    virtual bool Initialize(ID3D11Device* device) = 0;
    virtual void Update(float& elapsedTime) = 0;
    virtual void Render(ID3D11DeviceContext* immediateContext, float elapsedTime) = 0;
    virtual void ImGui() = 0;
    virtual void Uninitialize() = 0;

    bool IsLoading()
    {
        // Waiting for loading ...
        std::future_status status = future.wait_for(0ms);
        if (status == std::future_status::timeout)
        {
            return false;
        }

        return true;
    }

};

class SceneManager
{
private:
    bool m_isDebugMode;
    bool m_isStart;
public:
    Scene* m_activeScene;

    SceneManager()
    {
        m_isStart = true;
        m_activeScene = nullptr;
#if _DEBUG
        m_isDebugMode = true;
#else
        m_isDebugMode = false;
#endif
    }
    ~SceneManager() 
    {
        Uninitialize();
        delete m_activeScene;
    };

    void ChangeScene(Scene::SceneLabel scene);
    void Initialize(ID3D11Device* device);
    void Update(float& elapsedTime);
    void Render(ID3D11DeviceContext* immediateContext, float elapsedTime);
    void Uninitialize();

    inline bool& GetIsStart() { return m_isStart; }

    inline static SceneManager& GetInstance()
    {
        static SceneManager instance;
        return instance;
    }

};

#define ActivateScene SceneManager::GetInstance()
