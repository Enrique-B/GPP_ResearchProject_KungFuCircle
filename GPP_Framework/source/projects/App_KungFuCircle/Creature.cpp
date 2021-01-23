#include "stdafx.h"
#include "Creature.h"
#include "CreatureStates.h"

Creature::Creature()
	:SteeringAgent()
	,m_GridWeight{4}
{
	SetAttacks();
	m_pArrive = new Arrive{};
	m_pWander = new Wander{};
	m_pFlee = new Flee{};
	m_pStand = new Stand{};
	m_pEvade = new Evade{};
	std::vector<BlendedSteering::WeightedBehavior> weightedBehavior; 

	weightedBehavior.push_back(BlendedSteering::WeightedBehavior{ m_pArrive, SteeringBehavior::Arrive,0 });
	weightedBehavior.push_back(BlendedSteering::WeightedBehavior{ m_pWander, SteeringBehavior::Wander,0 });
	weightedBehavior.push_back(BlendedSteering::WeightedBehavior{ m_pFlee, SteeringBehavior::Flee,0 });
	weightedBehavior.push_back(BlendedSteering::WeightedBehavior{ m_pStand, SteeringBehavior::Stand,0 });
	weightedBehavior.push_back(BlendedSteering::WeightedBehavior{ m_pEvade, SteeringBehavior::Evade,0 });
	m_pBlendedSteering = new BlendedSteering{ weightedBehavior };
	SetStateMachine();
	SetMass(0);
	m_pArrive->SetSlowRadius(4);
	m_pArrive->SetArrivalRadius(0.25f);
	m_pFlee->SetFleeRadius(8);
	SetSteeringBehavior(m_pBlendedSteering);
	m_pEvade->SetFleeRadius(4);
	SetAutoOrient(true);
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
	SAFE_DELETE(m_pStand);
	SAFE_DELETE(m_pEvade);
	SAFE_DELETE(m_pBlendedSteering);
}

void Creature::Update(float dt)
{

	SteeringAgent::Update(dt);
	m_pStateMachine->Update(dt);
}

void Creature::Render(float dt)
{
	SteeringAgent::Render(dt);
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

	Elite::FSMTransition* pIsNotOutsideOfCircle = new IsNotOutsideOfCircle{};
	m_pTransitions.push_back(pIsNotOutsideOfCircle);

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
	m_pStateMachine->AddTransition(pIdleState, pLeaveFromCircleState, pIsNotOutsideOfCircle);
}
