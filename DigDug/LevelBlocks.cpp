#include "pch.h"
#include "LevelBlocks.h"

#include "GameObject.h"
#include "TextureComponent.h"
#include "BodyComponent.h"
#include "Scene.h"
#include "GameContext.h"

void Prefabs::CreateLevelBlock(dae::GameObject* out)
{
	if (out == nullptr)
		out = new dae::GameObject("LevelBlock");

	dae::BodyComponent* pBody = new dae::BodyComponent(b2BodyType::b2_staticBody);

	dae::BodyComponent::BoxFixtureDesc fixtureDesc{};
	fixtureDesc.halfWidth = 2.f;
	fixtureDesc.halfHeight = 2.f;
	fixtureDesc.isSensor = false;
	fixtureDesc.filter.categoryBits = 0x0002;
	fixtureDesc.filter.maskBits = 0x0001;
	// TODO: make bits not hardcoded

	std::vector<dae::BodyComponent::BoxFixtureDesc> boxFixtureDescs;
	fixtureDesc.center = { 2.f,2.f };
	boxFixtureDescs.push_back(fixtureDesc);
	fixtureDesc.center = { -2.f,-2.f };
	boxFixtureDescs.push_back(fixtureDesc);
	fixtureDesc.center = { 2.f,-2.f };
	boxFixtureDescs.push_back(fixtureDesc);
	fixtureDesc.center = { -2.f,2.f };
	boxFixtureDescs.push_back(fixtureDesc);

	pBody->SetBoxFixtures(boxFixtureDescs);

	out->AddComponent(pBody);

	SDL_Rect src{};
	src.h = src.w = 8;
	dae::TextureComponent* pTexture = new dae::TextureComponent("LevelBlock.gif", true, src);
	out->AddComponent(pTexture);
}

void Prefabs::CreateLevel(dae::Scene* pScene, const dae::GameSettings &gameSettings, const int blockSize, const int topBorder, const int bottomBorder)
{
	dae::GameObject* go = nullptr;

	const float halfSize = blockSize * .5f;

	float x = -halfSize;
	float y = topBorder + halfSize;

	const int width = gameSettings.WindowResolutionW / blockSize;
	const int height = gameSettings.WindowResolutionH / blockSize
		- topBorder / blockSize
		- bottomBorder / blockSize;

	for (int i = 0; i < height; ++i)
	{
		x = -halfSize;
		for (int j = 0; j < width; ++j)
		{
			Prefabs::CreateLevelBlock(go = new dae::GameObject("LevelBlock"));
			pScene->AddGameObject(go);
			x += blockSize;
			go->SetPosition(x, y);
		}
		y += blockSize;
	}
}