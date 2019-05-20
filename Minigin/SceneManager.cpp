#include "MiniginPCH.h"

#include "SceneManager.h"
#include "Scene.h"
#include "Renderer.h"
#include "GameContext.h"

namespace dae
{
	SceneManager::~SceneManager()
	{
		for (auto scene : m_pScenesMap)
		{
			delete scene.second;
		}
	}

	void SceneManager::Initialize(const GameContext &gameContext)
	{
		for (auto scene : m_pScenesMap)
		{
			scene.second->RootInitialize(gameContext);
		}
	}

	void SceneManager::FixedUpdate(float msPerFrame)
	{
		m_pActiveScene->FixedUpdate(msPerFrame);
	}

	void SceneManager::Update()
	{
		m_pActiveScene->RootUpdate();
	}

	void SceneManager::LateUpdate()
	{
		m_pActiveScene->RootLateUpdate();
	}

	void SceneManager::DestroyUpdate()
	{
		m_pActiveScene->DestroyUpdate();
	}

	void SceneManager::Render(const GameContext &gameContext, float extrapolate)
	{
		m_pActiveScene->Render(gameContext.Renderer, extrapolate);
	}

	void SceneManager::AddScene(Scene* pScene)
	{
		m_pScenesMap.insert({ pScene->GetName(),pScene });
		if (m_pActiveScene == nullptr)
			m_pActiveScene = pScene;
	}

	void SceneManager::RemoveScene(Scene* pScene)
	{
		m_pScenesMap.erase(pScene->GetName());
	}

	void SceneManager::RemoveScene(const std::string& sceneName)
	{
		m_pScenesMap.erase(sceneName);
	}

	void SceneManager::SetActiveScene(const std::string& sceneName)
	{
		m_pActiveScene = m_pScenesMap.at(sceneName);
	}

	Scene* SceneManager::GetActiveScene() const
	{
		return m_pActiveScene;
	}
}