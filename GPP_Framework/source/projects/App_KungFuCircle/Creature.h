#pragma once
#include "projects/Shared/BaseAgent.h"

struct Attack
{
	std::string attackName; 
	int AttackWeight;
};


class Creature :  public BaseAgent
{
public: 
	virtual void Update(float dt)override;
	virtual void Render(float dt)override;
	void SetGridWeight(int weight) { m_GridWeight = weight; };
	int GetGridWeight()const { return m_GridWeight; }
	const std::vector<Attack>& GetAttacks()const { return m_Attacks; }
private: 
	int m_GridWeight; 
	std::vector<Attack> m_Attacks;
};