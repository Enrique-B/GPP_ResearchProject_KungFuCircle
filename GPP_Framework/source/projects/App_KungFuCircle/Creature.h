#pragma once
#include "../App_Steering/SteeringAgent.h"
#include "../App_Steering/SteeringBehaviors.h"

struct Attack
{
	std::string attackName; 
	int AttackWeight;
	bool operator == (const Attack& attack)
	{
		return attackName == attack.attackName && AttackWeight == attack.AttackWeight;
	}
};

namespace Elite
{
	class FiniteStateMachine;
	class Blackboard;
	class FSMState;
	class FSMTransition;
}
class StageManager;

class Creature :  public SteeringAgent
{
public: 
	Creature(); 
	~Creature();
	virtual void Update(float dt)override;
	virtual void Render(float dt)override;
	void SetGridWeight(int weight) { m_GridWeight = weight; };
	int GetGridWeight()const { return m_GridWeight; }
	const std::vector<Attack>& GetAttacks()const { return m_Attacks; }
	int GetNodeIndex()const;
	void AddStageManager(StageManager* stageManager);

	void SetToWander();
	void SetToSeek(const Elite::Vector2& position); 
	void SetToFlee(const Elite::Vector2& position);
private: 
	int m_GridWeight; 
	std::vector<Attack> m_Attacks;
	Elite::FiniteStateMachine* m_pStateMachine;
	std::vector<Elite::FSMState*> m_pStates;
	std::vector<Elite::FSMTransition*> m_pTransitions;

	Seek* m_pSeek; 
	Wander* m_pWander;
	Flee* m_pFlee;
};