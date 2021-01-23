#pragma once
#include "../App_Steering/SteeringAgent.h"
#include "../App_Steering/SteeringBehaviors.h"
#include "../App_Steering/CombinedBehaviors/CombinedSteeringBehaviors.h"

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
	void AddStageManager(StageManager* stageManager);
	// getters and setters
	void SetGridWeight(int weight) { m_GridWeight = weight; };
	int GetGridWeight()const { return m_GridWeight; }
	const std::vector<Attack>& GetAttacks()const { return m_Attacks; }
	Flee* GetFlee()const { return m_pFlee; }

	int GetNodeIndex()const;
	BlendedSteering* GetBlendedSteering()const { return m_pBlendedSteering; }

private: 
	int m_GridWeight; 
	std::vector<Attack> m_Attacks;
	Elite::FiniteStateMachine* m_pStateMachine;
	std::vector<Elite::FSMState*> m_pStates;
	std::vector<Elite::FSMTransition*> m_pTransitions;

	BlendedSteering* m_pBlendedSteering;
	Stand* m_pStand;
	Arrive* m_pArrive; 
	Wander* m_pWander;
	Flee* m_pFlee;
	Evade* m_pEvade;
	void SetAttacks();
	void SetStateMachine();
};