#pragma once
#include "framework/EliteAI/EliteDecisionMaking/EliteFiniteStateMachine/EFiniteStateMachine.h"
#include "framework/EliteAI/EliteData/EBlackboard.h"
#include "StageManager.h"

#pragma region States
class AttackingState : public Elite::FSMState
{
public: 
	virtual void OnEnter(Elite::Blackboard* pBlackboard) 
	{
		if (m_pStageManager == nullptr)
		{
			pBlackboard->GetData("StageManager", m_pStageManager);
		}
		if (m_pCreature == nullptr)
		{
			pBlackboard->GetData("Creature", m_pCreature);
		}

		BlendedSteering* pBlendedSteering = m_pCreature->GetBlendedSteering();
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Arrive, 0.5f);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Wander, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Flee, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Stand, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Evade, 0.5f);
	}
	virtual void Update(Elite::Blackboard* pBlackboard, float deltaTime) 
	{
		const Elite::Vector2 playerPos = m_pStageManager->GetMiddlePoint();
		Elite::Vector2 closestEnemyPos; 
		BlendedSteering* pBlendedSteering = m_pCreature->GetBlendedSteering();
		pBlendedSteering->GetBehaviorFromEnum(SteeringBehavior::Arrive)->SetTarget(playerPos);
		if (m_pStageManager->GetClosestEnemyPos(m_pCreature, closestEnemyPos))
		{
			pBlendedSteering->GetBehaviorFromEnum(SteeringBehavior::Evade)->SetTarget(closestEnemyPos);
		}
	}
	virtual void OnExit(Elite::Blackboard* pBlackboard)
	{
		Attack attack;
		pBlackboard->GetData("currentAttack", attack);
		std::cout << attack.attackName << " AttackWeight: " << attack.AttackWeight << std::endl;
		m_pStageManager->RemoveCreatureFromGrid(m_pCreature);
	}
private: 
	StageManager* m_pStageManager = nullptr;
	Creature* m_pCreature = nullptr;
};

class GoToApproachCircle : public Elite::FSMState
{
	virtual void OnEnter(Elite::Blackboard* pBlackboard)
	{
		if (m_pStageManager == nullptr)
		{
			pBlackboard->GetData("StageManager", m_pStageManager);
		}
		if (m_pCreature == nullptr)
		{
			pBlackboard->GetData("Creature", m_pCreature);
		}
		BlendedSteering* pBlendedSteering = m_pCreature->GetBlendedSteering();
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Arrive, 0.5f);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Wander, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Flee, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Stand, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Evade, 0.5f);
	}
	virtual void Update(Elite::Blackboard* pBlackboard, float deltaTime)
	{
		Elite::Vector2 nodePosition{};
		BlendedSteering* pBlendedSteering = m_pCreature->GetBlendedSteering();
		if (m_pStageManager->GetPositionFromNodeIndex(m_pCreature->GetNodeIndex(), nodePosition))
		{
			pBlendedSteering->GetBehaviorFromEnum(SteeringBehavior::Arrive)->SetTarget(nodePosition);
			Elite::Vector2 closestEnemyPos;
			if (m_pStageManager->GetClosestEnemyPos(m_pCreature, closestEnemyPos))
			{
				pBlendedSteering->GetBehaviorFromEnum(SteeringBehavior::Evade);
			}
		}
	}
private: 
	StageManager* m_pStageManager = nullptr;
	Creature* m_pCreature = nullptr;
};

class LeaveFromCircle : public Elite::FSMState
{
public:
	virtual void OnEnter(Elite::Blackboard* pBlackboard)
	{
		if (m_pStageManager == nullptr)
		{
			pBlackboard->GetData("StageManager", m_pStageManager);
		}
		if (m_pCreature == nullptr)
		{
			pBlackboard->GetData("Creature", m_pCreature);
		}
		BlendedSteering* pBlendedSteering = m_pCreature->GetBlendedSteering();
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Arrive, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Wander, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Flee, 0.5f);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Stand, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Evade, 0.5f);
	}
	virtual void OnExit(Elite::Blackboard* pBlackboard) {}
	virtual void Update(Elite::Blackboard* pBlackboard, float deltaTime) 
	{
		const Elite::Vector2 playerPos = m_pStageManager->GetMiddlePoint();
		Elite::Vector2 closestEnemyPos;
		BlendedSteering* pBlendedSteering = m_pCreature->GetBlendedSteering();
		pBlendedSteering->GetBehaviorFromEnum(SteeringBehavior::Flee)->SetTarget(playerPos);
		if (m_pStageManager->GetClosestEnemyPos(m_pCreature, closestEnemyPos))
		{
			pBlendedSteering->GetBehaviorFromEnum(SteeringBehavior::Evade)->SetTarget(closestEnemyPos);
		}
	}
private:
	StageManager* m_pStageManager = nullptr;
	Creature* m_pCreature = nullptr;
};

class WanderState : public Elite::FSMState
{
public:
	virtual void OnEnter(Elite::Blackboard* pBlackboard)
	{
		if (m_pCreature == nullptr)
		{
			pBlackboard->GetData("Creature", m_pCreature);
		}
		BlendedSteering* pBlendedSteering = m_pCreature->GetBlendedSteering();
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Arrive, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Wander, 1);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Flee, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Stand, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Evade, 0);
	}
	virtual void OnExit(Elite::Blackboard* pBlackboard) {}
	virtual void Update(Elite::Blackboard* pBlackboard, float deltaTime) {}
private:
	Creature* m_pCreature{ nullptr };
};

class Idle : public Elite::FSMState
{
public:
	virtual void OnEnter(Elite::Blackboard* pBlackboard)  
	{
		if (m_pCreature == nullptr)
		{
			pBlackboard->GetData("Creature", m_pCreature);
		}
		BlendedSteering* pBlendedSteering = m_pCreature->GetBlendedSteering();
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Arrive, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Wander, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Flee, 0);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Stand, 1);
		pBlendedSteering->SetWeightByEnum(SteeringBehavior::Evade, 0);
		m_pCreature->SetAutoOrient(false);
	}
	virtual void OnExit(Elite::Blackboard* pBlackboard) 
	{
		m_pCreature->SetAutoOrient(true);
	}
	virtual void Update(Elite::Blackboard* pBlackboard, float deltaTime) {}
private: 
	Creature* m_pCreature{ nullptr };
};
#pragma endregion States

#pragma region Transitions
class IsDoneAttacking : public Elite::FSMTransition
{
public:
	virtual bool ToTransition(Elite::Blackboard* pBlackboard) const override
	{
		StageManager* pStageManager{ nullptr };
		Creature* pCreature{ nullptr };
		bool succeed = pBlackboard->GetData("StageManager", pStageManager) 
			&& pBlackboard->GetData("Creature", pCreature);
		if (!succeed || pStageManager == nullptr || pCreature == nullptr)
		{
			return false;
		}
		const Elite::Vector2 playerPos = pStageManager->GetMiddlePoint();
		const float minDistanceToLeave = 2.f;
		if (Elite::DistanceSquared(playerPos, pCreature->GetPosition()) < Elite::Square(minDistanceToLeave))
		{
			return true;
		}
		return false;
	}
};

class IsAbleToGoToTheApproachRadius : public Elite::FSMTransition
{
public:
	virtual bool ToTransition(Elite::Blackboard* pBlackboard)const override
	{
		StageManager* pStageManager{ nullptr };
		Creature* pCreature{ nullptr };
		bool succeed = pBlackboard->GetData("StageManager", pStageManager)
			&& pBlackboard->GetData("Creature", pCreature);
		if (!succeed || pStageManager == nullptr || pCreature == nullptr)
		{
			return false;
		}
		int nodeIndex{};
		if (pStageManager->RequestAccesToApproachCircle(pCreature, nodeIndex))
		{
			pBlackboard->ChangeData("NodeIndex", nodeIndex);
			return true;
		}
		return false;
	}
};

class IsAbleToAttack : public Elite::FSMTransition
{
public:
	virtual bool ToTransition(Elite::Blackboard* pBlackboard)const override
	{
		StageManager* pStageManager{ nullptr };
		Creature* pCreature{ nullptr };

		bool succeed = pBlackboard->GetData("StageManager", pStageManager)
			&& pBlackboard->GetData("Creature", pCreature);
		if (!succeed)
		{
			return false;
		}
		if (pStageManager == nullptr || pCreature == nullptr)
		{
			return false;
		}
		Attack attack; 
		if (pStageManager->RequestAttack(pCreature, attack))
		{
			pBlackboard->ChangeData("currentAttack", attack);
			return true;
		}
		return false;
	}
};

class IsOutsideOfCircle : public Elite::FSMTransition
{
public:
	virtual bool ToTransition(Elite::Blackboard* pBlackboard)const override
	{
		StageManager* pStageManager{ nullptr };
		Creature* pCreature{ nullptr };
		bool succeed = pBlackboard->GetData("StageManager", pStageManager)
			&& pBlackboard->GetData("Creature", pCreature);
		if (!succeed || pStageManager == nullptr || pCreature == nullptr)
		{
			return false;
		}
		return !pStageManager->IsPositionInOuterCircle(pCreature->GetPosition());
	}
};

class IsNotOutsideOfCircle : public Elite::FSMTransition
{
public:
	virtual bool ToTransition(Elite::Blackboard* pBlackboard)const override
	{
		StageManager* pStageManager{ nullptr };
		Creature* pCreature{ nullptr };
		bool succeed = pBlackboard->GetData("StageManager", pStageManager)
			&& pBlackboard->GetData("Creature", pCreature);
		if (!succeed || pStageManager == nullptr || pCreature == nullptr)
		{
			return false;
		}
		return pStageManager->IsPositionInOuterCircle(pCreature->GetPosition());
	}
};

#pragma endregion Transitions