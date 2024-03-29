#include "pch.h"
#include "Characters.h"
#include "DigDugStates.h"

#include <GameObject.h>
#include <AnimatedSpriteComponent.h>
#include <BodyComponent.h>
#include <GridComponent.h>
#include <FSMComponent.h>
#include <HealthComponent.h>
#include <Conversions.h>
#include <SubjectComponent.h>
#include <ObserverComponent.h>
#include <TextureComponent.h>

#include "HealthDisplay.h"
#include "Prefabs.h"
#include "DigDugLevel.h"
#include "EnemyStates.h"
#include "EnemyComponent.h"
#include "PlayerComponent.h"
#include "Settings.h"

namespace Characters
{
	unsigned short DigDug::m_CategoryBitsP1 = 0x001;
	unsigned short DigDug::m_CategoryBitsP2 = 0x002;
	unsigned short DigDug::m_CategoryBits = m_CategoryBitsP1 | m_CategoryBitsP2;
	unsigned short DigDug::m_CategoryBitsPump = 0x0064;

	void DigDug::CreateDigDugCharacter(dae::GameObject* &out, DigDugLevel* pScene, bool isPlayerOne)
	{
		if (out == nullptr)
			out = new dae::GameObject("DigDug");

		pScene->AddGameObject(out);

		dae::BodyComponent::BoxFixtureDesc fixtureDesc{};
		fixtureDesc.halfWidth = 7.f;
		fixtureDesc.halfHeight = 7.f;
		fixtureDesc.filter.categoryBits = isPlayerOne ? m_CategoryBitsP1 : m_CategoryBitsP2;
		fixtureDesc.filter.maskBits = Level::Rock::GetCategoryBits() | Level::LevelBlock::GetCategoryBits() |
			Enemy::GetCategoryBits();

		std::vector<dae::BodyComponent::BoxFixtureDesc> boxDescs;
		boxDescs.push_back(fixtureDesc);

		fixtureDesc.halfWidth = 7.7f;
		fixtureDesc.halfHeight = 7.7f;
		fixtureDesc.isSensor = true;

		boxDescs.push_back(fixtureDesc);

		dae::BodyComponent* pBody = new dae::BodyComponent(b2BodyType::b2_staticBody);
		pBody->SetBoxFixtures(boxDescs);
		out->AddComponent(pBody);

		// TODO: Make grid not hardcoded
		auto pGrid = new dae::GridComponent(16, { 8.f,24.f }, 224, 256, true);
		pScene->AddDebugDrawPointVec(dae::Conversions::GLM_To_SDL(pGrid->GetGrid()));
		out->AddComponent(pGrid);

		dae::FSMComponent* pFSM = new dae::FSMComponent();
		pFSM->SetGlobalState(new DigDugEx::States::GlobalState());
		pFSM->AddState(new DigDugEx::States::IdleState());
		pFSM->AddState(new DigDugEx::States::MovingState());
		pFSM->AddState(new DigDugEx::States::DeathState());
		pFSM->AddState(new DigDugEx::States::ThrowPumpState());
		pFSM->AddState(new DigDugEx::States::PumpingState());
		pFSM->AddState(new DigDugEx::States::SquishState());
		out->AddComponent(pFSM);

		SDL_Rect src{};
		src.h = src.w = 16;
		auto animatedSpriteComponent = new dae::AnimatedSpriteComponent("DigDug.gif", 1, true, src, 8, 2, false);
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Walking), 0, 2, 0.1f));
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Digging), 2, 2, 0.1f));
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::SquishH), 4, 1, 0.1f));
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::SquishV), 5, 1, 0.1f));
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Dying), 6, 5, 0.5f));
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::ThrowPump), 11, 1, 0.1f));
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Pumping), 14, 2, 0.3f));
		animatedSpriteComponent->SetActiveClip(to_integral(AnimationClips::Walking));
		out->AddComponent(animatedSpriteComponent);

		dae::GameObject* healthDisplay = nullptr;
		HealthDisplay::CreateHealthDisplay(healthDisplay, isPlayerOne ? "Player1" : "Player2", isPlayerOne);
		pScene->AddGameObject(healthDisplay);

		if(isPlayerOne)
			healthDisplay->SetPosition(3.f, 16.f, dae::Anchor::BottomLeft);
		else
			healthDisplay->SetPosition(35.f, 16.f, dae::Anchor::BottomRight);

		auto pSubject = new dae::SubjectComponent(isPlayerOne ? "Player1" : "Player2");
		pSubject->AddObserver(healthDisplay->GetComponent<dae::ObserverComponent>());
		pSubject->AddObserver(pScene);
		out->AddComponent(pSubject);

		out->AddComponent(new dae::HealthComponent(1.f, 3));

		const std::string hAxisMapping = isPlayerOne ? Settings::P1HAxis : Settings::P2HAxis;
		const std::string vAxisMapping = isPlayerOne ? Settings::P1VAxis : Settings::P2VAxis;
		const std::string pumpMapping = isPlayerOne ? Settings::P1Pump : Settings::P2Pump;

		out->AddComponent(new PlayerComponent(hAxisMapping, vAxisMapping, pumpMapping));

		// Pump
		auto pump = new dae::GameObject("Pump");
		out->AddChild(pump);
		pump->SetLocalPosition(8.f, 0.f);

		SDL_Rect pumpSrc{};
		pumpSrc.y = 96;
		pumpSrc.h = -1;
		pumpSrc.w = -1;
		pump->AddComponent(new dae::TextureComponent("DigDug.gif", 2, true, pumpSrc));

		dae::BodyComponent* pPumpBody = new dae::BodyComponent(b2BodyType::b2_dynamicBody);
		pump->AddComponent(pPumpBody);
	}

	unsigned short Enemy::m_CategoryBits = 0x032;

	void Enemy::CreateFygarCharacter(dae::GameObject* &out, std::vector<dae::GameObject*> pPlayers, DigDugLevel* pScene, bool isPlayer)
	{
		if (out == nullptr)
			out = new dae::GameObject(isPlayer ? "Player2" : "Fygar");

		pScene->AddGameObject(out);

		dae::BodyComponent::BoxFixtureDesc fixtureDesc{};

		fixtureDesc.halfWidth = 7.7f;
		fixtureDesc.halfHeight = 7.7f;
		fixtureDesc.isSensor = true;
		fixtureDesc.filter.categoryBits = m_CategoryBits;
		fixtureDesc.filter.maskBits = Level::Rock::GetCategoryBits() | Level::LevelBlock::GetCategoryBits()
			| DigDug::GetCategoryBits() | DigDug::GetPumpCategoryBits();

		dae::BodyComponent* pBody = new dae::BodyComponent(b2BodyType::b2_dynamicBody);
		pBody->SetBoxFixture(fixtureDesc);
		out->AddComponent(pBody);

		// TODO: Make grid not hardcoded
		auto pGrid = new dae::GridComponent(16, { 8.f,24.f }, 224, 256, true);
		pScene->AddDebugDrawPointVec(dae::Conversions::GLM_To_SDL(pGrid->GetGrid()));
		out->AddComponent(pGrid);

		SDL_Rect src{};
		src.h = src.w = 16;
		auto animatedSpriteComponent = new dae::AnimatedSpriteComponent("Fygar.gif", 1, true, src, 10, 2, false);
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Walking), 0, 2, 0.2f));
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Ghosting), 2, 2, 0.1f));
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(Fygar::AnimationClips::FireBreath), 4, 2, 0.1f));
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Squish), 6, 1, 0.1f));
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Pumped), 7, 1, 0.1f));
		SDL_Rect balloonSrc{};
		balloonSrc.h = 24;
		balloonSrc.w = 32;
		balloonSrc.x = 0;
		balloonSrc.y = 64;
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Ballooning), 3, 0.1f, 1,
			balloonSrc));
		animatedSpriteComponent->SetActiveClip(to_integral(AnimationClips::Walking));
		out->AddComponent(animatedSpriteComponent);


		// Fire
		auto fire = new dae::GameObject("Fire");
		out->AddChild(fire);
		fire->SetLocalPosition(8.f, 0.f);

		SDL_Rect fireSrc{};
		fireSrc.h = 16;
		fireSrc.w = 96;
		animatedSpriteComponent = new dae::AnimatedSpriteComponent("FygarFire.gif", 2, true, fireSrc, 1, 3, false);

		fireSrc.h = 16;
		fireSrc.w = 16;
		SDL_Rect fireGrow{};
		fireGrow.h = 0;
		fireGrow.w = 16;
		animatedSpriteComponent->AddClip(
			dae::AnimatedSpriteClip(to_integral(Fygar::AnimationClips::Fire), 3, .3f, 3, fireSrc, fireGrow));
		fire->AddComponent(animatedSpriteComponent);

		dae::BodyComponent* pFireBody = new dae::BodyComponent(b2BodyType::b2_dynamicBody);
		fire->AddComponent(pFireBody);

		// Player or enemy ?
		if (isPlayer)
		{
			dae::FSMComponent* pFSM = new dae::FSMComponent();
			pFSM->SetGlobalState(new EnemyEx::States::GlobalState(isPlayer));
			pFSM->AddState(new DigDugEx::States::IdleState());
			pFSM->AddState(new DigDugEx::States::MovingState());
			pFSM->AddState(new EnemyEx::States::DeathState());
			pFSM->AddState(new FygarEx::States::FireBreathingState(isPlayer));
			out->AddComponent(pFSM);

			dae::GameObject* healthDisplay = nullptr;
			HealthDisplay::CreateHealthDisplay(healthDisplay, "Player2", false);
			pScene->AddGameObject(healthDisplay);

			healthDisplay->SetPosition(35.f, 16.f, dae::Anchor::BottomRight);

			auto pSubject = new dae::SubjectComponent("Player2");
			pSubject->AddObserver(healthDisplay->GetComponent<dae::ObserverComponent>());
			pSubject->AddObserver(pScene);
			out->AddComponent(pSubject);

			out->AddComponent(new dae::HealthComponent(1.f, 3));

			out->AddComponent(new PlayerComponent(Settings::P2HAxis, Settings::P2VAxis, Settings::P2Pump));
		}
		else
		{
			dae::FSMComponent* pFSM = new dae::FSMComponent();
			pFSM->SetGlobalState(new EnemyEx::States::GlobalState(isPlayer));
			pFSM->AddState(new EnemyEx::States::MovingState(true));
			pFSM->AddState(new EnemyEx::States::DeathState());
			pFSM->AddState(new EnemyEx::States::HitState());
			pFSM->AddState(new EnemyEx::States::GhostState());
			pFSM->AddState(new FygarEx::States::FireBreathingState(isPlayer));
			out->AddComponent(pFSM);

			out->AddComponent(new EnemyComponent(pPlayers));
		}
	}

	void Enemy::CreatePookaCharacter(dae::GameObject* &out, std::vector<dae::GameObject*> pPlayers, DigDugLevel* pScene)
	{
		if (out == nullptr)
			out = new dae::GameObject("Pooka");

		dae::BodyComponent::BoxFixtureDesc fixtureDesc{};
		fixtureDesc.halfWidth = 7.7f;
		fixtureDesc.halfHeight = 7.7f;
		fixtureDesc.isSensor = true;
		fixtureDesc.filter.categoryBits = m_CategoryBits;
		fixtureDesc.filter.maskBits = Level::Rock::GetCategoryBits() | Level::LevelBlock::GetCategoryBits()
			| DigDug::GetCategoryBits() | DigDug::GetPumpCategoryBits();

		dae::BodyComponent* pBody = new dae::BodyComponent(b2BodyType::b2_dynamicBody);
		pBody->SetBoxFixture(fixtureDesc);
		out->AddComponent(pBody);

		// TODO: Make grid not hardcoded
		auto pGrid = new dae::GridComponent(16, { 8.f,24.f }, 224, 256, true);
		pScene->AddDebugDrawPointVec(dae::Conversions::GLM_To_SDL(pGrid->GetGrid()));
		out->AddComponent(pGrid);

		SDL_Rect src{};
		src.h = src.w = 16;
		auto animatedSpriteComponent = new dae::AnimatedSpriteComponent("Pooka.gif", 1, true, src, 10, 2, false);
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Walking), 0, 2, 0.2f));
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Ghosting), 2, 2, 0.1f));
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Squish), 4, 1, 0.1f));
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Pumped), 5, 1, 0.1f));
		SDL_Rect balloonSrc{};
		balloonSrc.h = 24;
		balloonSrc.w = 32;
		balloonSrc.x = 0;
		balloonSrc.y = 48;
		animatedSpriteComponent->AddClip(dae::AnimatedSpriteClip(to_integral(AnimationClips::Ballooning), 3, 0.1f, 1,
			balloonSrc));
		animatedSpriteComponent->SetActiveClip(to_integral(AnimationClips::Walking));
		out->AddComponent(animatedSpriteComponent);

		dae::FSMComponent* pFSM = new dae::FSMComponent();
		pFSM->SetGlobalState(new EnemyEx::States::GlobalState(false));
		pFSM->AddState(new EnemyEx::States::MovingState(false));
		pFSM->AddState(new EnemyEx::States::DeathState());
		pFSM->AddState(new EnemyEx::States::HitState());
		pFSM->AddState(new EnemyEx::States::GhostState());
		out->AddComponent(pFSM);

		out->AddComponent(new EnemyComponent(pPlayers));
	}
}