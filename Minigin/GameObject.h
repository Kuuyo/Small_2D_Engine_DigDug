#pragma once
#include "Scene.h"

#pragma warning (push)
#pragma warning (disable:4201)
#include <glm/vec3.hpp>
#pragma warning (pop)

// TODO: Add possibility to add GameObject as a child to a GameObject ?

namespace dae
{
	class BaseComponent;
	class TransformComponent;
	struct GameContext;

	class GameObject final
	{
	public:
		GameObject();
		GameObject(std::string &&tag);
		~GameObject();

		void Initialize(const GameContext &gameContext);
		void Update(const GameContext &gameContext);

		void AddComponent(BaseComponent* component);

		void SetPosition(float x = 0, float y = 0, float z = 0);
		void SetPosition(glm::vec3 pos);
		const glm::vec3& GetPosition() const;
		Scene* GetScene() const;
		std::string GetTag() const { return m_Tag; }

		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

#pragma region Templates
		// based on OverLord Engine
		template <class T>
		bool HasComponent()
		{
			return GetComponent<T>() != nullptr;
		}

		template <class T>
		T* GetComponent()
		{
			const type_info& ti = typeid(T);
			for (auto* component : m_pVecComponents)
			{
				if (component && typeid(*component) == ti)
					return static_cast<T*>(component);
			}

			return nullptr;
		}
#pragma endregion

	private:
		friend void Scene::AddGameObject(const std::shared_ptr<GameObject>& object);

		void SetScene(Scene* pScene);

		std::string m_Tag;
		std::vector<BaseComponent*> m_pVecComponents;
		TransformComponent* m_pTransform;
		Scene* m_pScene;
	};
}
