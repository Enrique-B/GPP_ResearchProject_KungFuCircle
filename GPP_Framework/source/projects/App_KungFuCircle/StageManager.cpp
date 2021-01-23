#include "stdafx.h"
#include "StageManager.h"
#include "KungFuGrid.h"
#include "Creature.h"

StageManager::StageManager()
	:m_CurrentGridCapacity{0}
	,m_CurrentAttackCapicty{0}
	,m_pPlayerKungFuGrid{nullptr}
{
}

StageManager::~StageManager()
{
}

// normally you do this on an collision object and add the creature on a beginoverlap
// and release them on an endoverlap
void StageManager::Update(const std::vector<Creature*>& pCreatures, float dt)
{
	for (Creature* pCreature : pCreatures)
	{
		if (Elite::DistanceSquared(pCreature->GetPosition(), m_pPlayerKungFuGrid->GetMiddlePos())
			< Elite::Square(m_pPlayerKungFuGrid->GetWaitingCircleRadius()))
		{
			auto it = std::find(m_pCreaturesInWaitingCircle.begin(), m_pCreaturesInWaitingCircle.end(), pCreature);
			// if it's not in the circle 
			if (it == m_pCreaturesInWaitingCircle.end())
			{
				m_pCreaturesInWaitingCircle.push_back(pCreature);
			}
		}
		else
		{
			auto it = std::find(m_pCreaturesInWaitingCircle.begin(), m_pCreaturesInWaitingCircle.end(), pCreature);
			if (it != m_pCreaturesInWaitingCircle.end())
			{
				m_pCreaturesInWaitingCircle.erase(std::remove(m_pCreaturesInWaitingCircle.begin(), m_pCreaturesInWaitingCircle.end(), pCreature));
			}
		}
	}
}

void StageManager::SetPlayerKungFuGrid(KungFuGrid* pGrid)
{
	m_pPlayerKungFuGrid = pGrid;
}

bool StageManager::RequestAccesToApproachCircle(Creature* pCreature, int& nodeIndex)
{
	// looking if it's in the waiting circle before allowing it in the approachCircle
	const float distanceSquared{ Elite::DistanceSquared(pCreature->GetPosition(), m_pPlayerKungFuGrid->GetMiddlePos()) };
	const float squaredWaitingCircle{ Elite::Square(m_pPlayerKungFuGrid->GetWaitingCircleRadius()) };
	const float squareApproachCircle{ Elite::Square(m_pPlayerKungFuGrid->GetApproachCircleRadius()) };
	if (distanceSquared > squaredWaitingCircle 	|| distanceSquared < squareApproachCircle)
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
	m_CurrentGridCapacity += pCreature->GetGridWeight();
	return true;
}

bool StageManager::RequestAttack(Creature* pCreature, Attack& attack)
{
	// let them attack 
	const float maxDistance{1.f};
	Elite::Vector2 nodeMiddlePos;
	GetPositionFromNodeIndex(pCreature->GetNodeIndex(), nodeMiddlePos);
	if (Elite::DistanceSquared(pCreature->GetPosition(), nodeMiddlePos) > Elite::Square(maxDistance))
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
	attack = possibleAttacks[Elite::randomInt(possibleAttacks.size())];
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
	const float offset{0.5f};
	return Elite::DistanceSquared(m_pPlayerKungFuGrid->GetMiddlePos(), position) 
		< Elite::Square(m_pPlayerKungFuGrid->GetApproachCircleRadius() + offset);
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

bool StageManager::GetClosestEnemyPos(Creature* pCreature , Elite::Vector2& closestEnemyPos) const
{
	if (m_pCreaturesInWaitingCircle.empty())
	{
		return false;
	}
	float closestDistance = FLT_MAX;
	const Elite::Vector2 creaturePos = pCreature->GetPosition();
	bool hasFoundValidPosition = false;
	for (Creature* pC : m_pCreaturesInWaitingCircle)
	{
		if (pC != pCreature)
		{
			const float distance = Elite::DistanceSquared(pC->GetPosition(), creaturePos);
			if (distance < closestDistance)
			{
				closestDistance = distance; 
				closestEnemyPos = pC->GetPosition();
				hasFoundValidPosition = true;
			}
		}
	}
	return hasFoundValidPosition;
}
