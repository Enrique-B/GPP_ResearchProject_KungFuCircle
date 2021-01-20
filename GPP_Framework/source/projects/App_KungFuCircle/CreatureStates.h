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
		if (pStageManager == nullptr)
		{
			pBlackboard->GetData("StageManager", pStageManager);
		}
		pBlackboard->GetData("Creature", pCurrentCreature);
	}
	virtual void Update(Elite::Blackboard* pBlackboard, float deltaTime) 
	{
		Elite::Vector2 playerPos = pStageManager->GetMiddlePoint();
		pCurrentCreature->SetToSeek(playerPos);
	}
	virtual void OnExit(Elite::Blackboard* pBlackboard)
	{
		Attack attack;
		pBlackboard->GetData("currentAttack", attack);
		std::cout << attack.attackName << " AttackWeight: " << attack.AttackWeight << std::endl;
		pStageManager->RemoveCreatureFromGrid(pCurrentCreature);
	}
private: 
	StageManager* pStageManager = nullptr;
	Creature* pCurrentCreature = nullptr;
};

class GoToApproachCircle : public Elite::FSMState
{
	virtual void OnEnter(Elite::Blackboard* pBlackboard)
	{
		if (pStageManager == nullptr)
		{
			pBlackboard->GetData("StageManager", pStageManager);
		}
		pBlackboard->GetData("Creature", pCurrentCreature);
	}
	virtual void Update(Elite::Blackboard* pBlackboard, float deltaTime)
	{
		int index{};
		bool succeed = pBlackboard->GetData("NodeIndex", index);
		if (!succeed)
		{
			return;
		}
		Elite::Vector2 position{};
		if (pStageManager->GetPositionFromNodeIndex(index, position))
		{
			pCurrentCreature->SetToSeek(position);
		}
	}
private: 
	StageManager* pStageManager = nullptr;
	Creature* pCurrentCreature = nullptr;
};

class LeaveFromCircle : public Elite::FSMState
{
public:
	virtual void OnEnter(Elite::Blackboard* pBlackboard)
	{
		if (pStageManager == nullptr)
		{
			pBlackboard->GetData("StageManager", pStageManager);
		}
		pBlackboard->GetData("Creature", pCurrentCreature);
	}
	virtual void OnExit(Elite::Blackboard* pBlackboard) {}
	virtual void Update(Elite::Blackboard* pBlackboard, float deltaTime) 
	{
		const Elite::Vector2 playerPos = pStageManager->GetMiddlePoint();
		pCurrentCreature->SetToFlee(playerPos);
	}
private:
	StageManager* pStageManager = nullptr;
	Creature* pCurrentCreature = nullptr;
};

class Idle : public Elite::FSMState
{
public:
	virtual void OnEnter(Elite::Blackboard* pBlackboard) 
	{
		if (pCreature == nullptr)
		{
			pBlackboard->GetData("Creature", pCreature);
		}
	}
	virtual void OnExit(Elite::Blackboard* pBlackboard) {}
	virtual void Update(Elite::Blackboard* pBlackboard, float deltaTime) 
	{
		pCreature->SetToWander();
	}
private: 
	Creature* pCreature{ nullptr };
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
		const float minDistanceToLeave = Elite::Square(3.0f);
		if (Elite::DistanceSquared(playerPos, pCreature->GetPosition()) < minDistanceToLeave)
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
		bool isInOuterCircle = pStageManager->IsPositionInOuterCircle(pCreature->GetPosition());
		return isInOuterCircle;
	}
};
#pragma endregion Transitions