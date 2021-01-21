#include "stdafx.h"
#include "Creature.h"
#include "CreatureStates.h"

Creature::Creature()
	:SteeringAgent()
	,m_GridWeight{4}
{
	SetAttacks();
	SetStateMachine();
	SetMass(1);
	m_pArrive = new Arrive{};
	m_pWander = new Wander{};
	m_pFlee = new Flee{};
	m_pArrive->SetSlowRadius(7);
	m_pArrive->SetArrivalRadius(3);
	m_pFlee->SetFleeRadius(6);
}

Creature::~Creature()
{
	SAFE_DELETE(m_pStateMachine);
	for (Elite::FSMState* pState : m_pStates)
	{
		SAFE_DELETE(pState);
	}
	for (Elite::FSMTransition* pTransition : m_pTransitions)
	{
		SAFE_DELETE(pTransition);
	}

	SAFE_DELETE(m_pFlee);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pArrive);
}

void Creature::Update(float dt)
{
	SteeringAgent::Update(dt);
	m_pStateMachine->Update(dt);
}

void Creature::Render(float dt)
{
	BaseAgent::Render(dt);
}

int Creature::GetNodeIndex() const
{
	int nodeIndex{};
	m_pStateMachine->GetBlackboard()->GetData("NodeIndex", nodeIndex);
	return nodeIndex;
}

void Creature::AddStageManager(StageManager* stageManager)
{
	m_pStateMachine->GetBlackboard()->AddData("StageManager", stageManager);
}

void Creature::SetToWander()
{
	SetSteeringBehavior(m_pWander);
}

void Creature::SetToArrive(const Elite::Vector2& position)
{
	m_pArrive->SetTarget(position);	
	SetSteeringBehavior(m_pArrive);
}

void Creature::SetToFlee(const Elite::Vector2& position)
{
	m_pFlee->SetTarget(position);
	SetSteeringBehavior(m_pFlee);
}

void Creature::SetAttacks()
{
	Attack firstAttack;
	firstAttack.attackName = "LightAttack";
	firstAttack.AttackWeight = 2;
	m_Attacks.push_back(firstAttack);
	Attack secondAttack;
	secondAttack.attackName = "HeavyAttack";
	secondAttack.AttackWeight = 4;
	m_Attacks.push_back(secondAttack);
}

void Creature::SetStateMachine()
{
	Elite::FSMState* pIdleState = new Idle{};
	m_pStates.push_back(pIdleState);
	Elite::FSMState* pGoToApproachCircleState = new GoToApproachCircle{};
	m_pStates.push_back(pGoToApproachCircleState);
	Elite::FSMState* pLeaveFromCircleState = new LeaveFromCircle{};
	m_pStates.push_back(pLeaveFromCircleState);
	Elite::FSMState* pAttackingState = new AttackingState{};
	m_pStates.push_back(pAttackingState);

	Elite::FSMTransition* pIsDoneAttacking = new IsDoneAttacking{};
	m_pTransitions.push_back(pIsDoneAttacking);
	Elite::FSMTransition* pIsAbleToGoToTheApproachRadius = new IsAbleToGoToTheApproachRadius{};
	m_pTransitions.push_back(pIsAbleToGoToTheApproachRadius);
	Elite::FSMTransition* pIsAbleToAttack = new IsAbleToAttack{};
	m_pTransitions.push_back(pIsAbleToAttack);
	Elite::FSMTransition* pIsOutsideOfCircle = new IsOutsideOfCircle{};
	m_pTransitions.push_back(pIsOutsideOfCircle);

	Elite::Blackboard* pBlackboard = new Elite::Blackboard{};
	pBlackboard->AddData("Creature", this);
	Attack currentAttack;
	pBlackboard->AddData("currentAttack", currentAttack);
	int index{};
	pBlackboard->AddData("NodeIndex", index);

	m_pStateMachine = new Elite::FiniteStateMachine{ pIdleState,pBlackboard };

	m_pStateMachine->AddTransition(pIdleState, pGoToApproachCircleState, pIsAbleToGoToTheApproachRadius);
	m_pStateMachine->AddTransition(pGoToApproachCircleState, pAttackingState, pIsAbleToAttack);
	m_pStateMachine->AddTransition(pAttackingState, pLeaveFromCircleState, pIsDoneAttacking);
	m_pStateMachine->AddTransition(pLeaveFromCircleState, pIdleState, pIsOutsideOfCircle);
}
