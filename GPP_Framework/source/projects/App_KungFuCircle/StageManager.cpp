#include "stdafx.h"
#include "StageManager.h"
#include "KungFuGrid.h"

StageManager::StageManager()
	:m_CurrentGridCapacity{0}
	,m_CurrentAttackCapicty{0}
	,m_pPlayerKungFuGrid{nullptr}
{
}

StageManager::~StageManager()
{
}

void StageManager::SetPlayerKungFuGrid(KungFuGrid* pGrid)
{
	m_pPlayerKungFuGrid = pGrid;
}

bool StageManager::RequestAccesToApproachCircle(Creature* pCreature, int nodeIndex)
{
	// looking if it's in the waiting circle before allowing it in the approachCircle
	const float squaredDistance = Elite::DistanceSquared(pCreature->GetPosition(), m_pPlayerKungFuGrid->GetMiddlePos());
	if (Elite::Square(m_pPlayerKungFuGrid->GetWaitingCircleRadius()) < squaredDistance)
	{
		return false;
	}
	//looking if it can fit in the grid 
	if (m_CurrentGridCapacity + pCreature->GetGridWeight() > m_pPlayerKungFuGrid->GetGridCapacity())
	{
		return false;
	}
	if (!m_pPlayerKungFuGrid->GetClosestNode(pCreature->GetPosition(), nodeIndex))
	{
		return false;
	}
	return true;
}

bool StageManager::RequestAttack(Creature* pCreature, Attack& attack)
{
	// let them attack 
	const float maxDistance{Elite::Square(m_pPlayerKungFuGrid->GetApproachCircleRadius())};
	if (Elite::DistanceSquared(pCreature->GetPosition(), m_pPlayerKungFuGrid->GetMiddlePos()) > maxDistance)
	{
		return false;
	}
	std::vector<Attack> possibleAttacks;
	for (const Attack& attack : pCreature->GetAttacks())
	{
		if (attack.AttackWeight + m_CurrentAttackCapicty <= m_pPlayerKungFuGrid->GetAttackCapacity())
		{
			possibleAttacks.push_back(attack);
		}
	}
	// if there are no possible attacks just wait
	if (possibleAttacks.empty())
	{
		return false;
	}
	// if there are possible attacks to be done we do a random 
	attack = possibleAttacks[Elite::randomInt(possibleAttacks.size() - 1)];
	m_CurrentAttackCapicty += attack.AttackWeight;
	m_pCreaturesOnGrid[pCreature] = attack;

	return true;
}

bool StageManager::GetPositionFromNodeIndex(int nodeIndex, Elite::Vector2& position)
{
	return m_pPlayerKungFuGrid->GetPositionFromNodeIndex(nodeIndex, position);
}

bool StageManager::IsPositionInOuterCircle(const Elite::Vector2& position)
{
	return Elite::DistanceSquared(m_pPlayerKungFuGrid->GetMiddlePos(), position) > Elite::Square(m_pPlayerKungFuGrid->GetApproachCircleRadius());
}

void StageManager::RemoveCreatureFromGrid(Creature* pCreature)
{
	std::unordered_map<Creature*, Attack>::iterator it = m_pCreaturesOnGrid.find(pCreature);
	if (it != m_pCreaturesOnGrid.end()) // it's in the map 
	{
		m_CurrentAttackCapicty -= it->second.AttackWeight;
		m_CurrentGridCapacity -= it->first->GetGridWeight();
		m_pPlayerKungFuGrid->SetNodeUnoccupied(it->first->GetNodeIndex());
		m_pCreaturesOnGrid.erase(it);
	}
}

const Elite::Vector2& StageManager::GetMiddlePoint() const
{
	return m_pPlayerKungFuGrid->GetMiddlePos();
}
