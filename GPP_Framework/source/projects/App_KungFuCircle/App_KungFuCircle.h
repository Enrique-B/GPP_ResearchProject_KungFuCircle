#pragma once
#include "framework/EliteInterfaces/EIApp.h"
#include "KungFuGrid.h"
#include <vector>
class Creature;
class Player; 
class StageManager;
class App_KungFuCircle :public IApp
{
public: 
	//Constructor & Destructor
	App_KungFuCircle() = default;
	virtual ~App_KungFuCircle() final;

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;
private: 
	std::vector<Creature*> m_pEnemyCreatures; 
	Player* m_pPlayer;
	KungFuGrid* m_KungFuGrid;
	StageManager* m_pStageManager;
	float AttackRange = 5; 
	void IMGUIUpdate();
};

