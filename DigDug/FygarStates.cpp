#include "pch.h"
#include "FygarStates.h"

#include <GameContext.h>
#include <InputManager.h>
#include <GameObject.h>
#include <BodyComponent.h>
#include <GridComponent.h>
#include <AnimatedSpriteComponent.h>
#include <HealthComponent.h>
#include <SubjectComponent.h>
#include <Time.h>
#include <Scene.h>

#pragma warning(push)
#pragma warning (disable:4201)
#include <glm/detail/func_common.hpp>
#pragma warning(pop)

#include "Characters.h"

namespace Characters
{
	namespace FygarEx
	{
		namespace States
		{
			void GlobalState::Initialize(const dae::SceneContext &)
			{

			}

			void GlobalState::OnEnter(const dae::SceneContext &)
			{

			}

			void GlobalState::Update(const dae::SceneContext &)
			{
				if (!IsActiveState<DeathState>())
				{
					auto contactList = GetGameObject()->GetComponent<dae::BodyComponent>()->GetContactList();
					if (contactList != nullptr)
					{
						if (reinterpret_cast<dae::BodyComponent*>(contactList->other->GetUserData())->
							GetGameObject()->GetTag() == "Rock" && contactList->other->GetType() == b2BodyType::b2_dynamicBody)
						{
							auto asc = GetGameObject()->GetComponent<dae::AnimatedSpriteComponent>();
							asc->SetActiveClip(to_integral(Characters::Fygar::AnimationClips::Squish));
							asc->PlayOnce();

							ChangeState<DeathState>();
							return;
						}
					}
				}
			}

			void GlobalState::OnExit(const dae::SceneContext &)
			{

			}


			MovingState::MovingState()
			{
			}

			MovingState::~MovingState()
			{
			}

			void MovingState::Initialize(const dae::SceneContext &)
			{
				// https://stackoverflow.com/questions/686353/random-float-number-generation
				m_RandomEngine = std::mt19937(m_RandomDevice());
			}

			void MovingState::OnEnter(const dae::SceneContext &)
			{
				auto asc = GetGameObject()->GetComponent<dae::AnimatedSpriteComponent>();
				asc->SetActiveClip(to_integral(Characters::Fygar::AnimationClips::Walking));
				asc->Play();
			}

			void MovingState::Update(const dae::SceneContext &sceneContext)
			{
				m_Timer += sceneContext.GameContext->Time->GetDeltaTime();

				if (m_Timer > GetRandomFloat(1.f, 5.f))
				{
					m_Horizontal = GetRandomFloat(-1.f, 1.f);
					m_Vertical = GetRandomFloat(-1.f, 1.f);
					m_Timer = 0.f;
				}

				LogDebugC(std::to_string(m_Horizontal));
				LogDebugC(std::to_string(m_Vertical));


				if (m_Horizontal == 0.f && m_Vertical == 0.f)
				{
					m_Horizontal = 1.f;
				}

				glm::vec2 direction{ m_Horizontal,m_Vertical };

				if (direction.x != 0.f && direction.y != 0.f)
				{
					if (glm::abs(direction.x) < glm::abs(direction.y))
						direction.y = 0;
					else
						direction.x = 0;
				}

				auto gameObject = GetGameObject();

				auto pos = gameObject->GetPosition();
				auto target = gameObject->GetComponent<dae::GridComponent>()->GetNextGridPoint(pos, direction);

				auto vector = target - pos;

				if (vector.x != 0.f && vector.y != 0.f)
				{
					if (glm::abs(vector.x) < glm::abs(vector.y))
						vector.y = 0;
					else
						vector.x = 0;
				}

				gameObject->GetComponent<dae::TransformComponent>()->SetOrientationX(vector.x);

				vector += pos;

				gameObject->GetComponent<dae::BodyComponent>()->MoveToTarget(vector, 32.f);
			}

			void MovingState::OnExit(const dae::SceneContext &)
			{
				GetGameObject()->GetComponent<dae::BodyComponent>()->SetLinearVelocity(0.f, 0.f);
				GetGameObject()->GetComponent<dae::AnimatedSpriteComponent>()->Stop();
			}

			float MovingState::GetRandomFloat(float min, float max)
			{
				std::uniform_real_distribution<float> dist(min, max);
				return glm::round(dist(m_RandomEngine));
			}



			void DeathState::Initialize(const dae::SceneContext &)
			{

			}

			void DeathState::OnEnter(const dae::SceneContext &)
			{
				auto asc = GetGameObject()->GetComponent<dae::AnimatedSpriteComponent>();
				asc->SetActiveClip(to_integral(Characters::DigDug::AnimationClips::Dying)); // TODO:
				asc->PlayOnce();
			}

			void DeathState::Update(const dae::SceneContext &sceneContext)
			{
				const auto asc = GetGameObject()->GetComponent<dae::AnimatedSpriteComponent>();

				if (!asc->IsPlaying())
				{
					asc->HideTexture();

					m_Timer += sceneContext.GameContext->Time->GetDeltaTime();
					if (m_Timer >= m_Duration)
					{
						GetGameObject()->GetScene()->RemoveGameObject(GetGameObject());
					}
				}
			}

			void DeathState::OnExit(const dae::SceneContext &)
			{
				m_Timer = 0.f;
			}
		}
	}
}