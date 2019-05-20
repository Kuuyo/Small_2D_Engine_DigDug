#pragma once
#include <Scene.h>

class MainMenu final : public dae::Scene
{
public:
	MainMenu();
	~MainMenu();

protected:
	void Initialize(const dae::GameContext &gameContext) override;
	void Update(const dae::GameContext &gameContext) override;

	void SelectGameObject(dae::GameObject* gameObject);
	void UnSelectGameObject(dae::GameObject* gameObject);

private:
	std::vector<dae::GameObject*> m_MenuSelectableObjects;
	int m_CurrentlySelected;
	float m_PreviousAxis;
};

