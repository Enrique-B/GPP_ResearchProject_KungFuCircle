#include "stdafx.h"
#include "KungFuGrid.h"
#include "Creature.h"

KungFuGrid::KungFuGrid()
	:m_MiddlePos{}
	, m_CurrentGridCapacity{0}
	, m_CurrentAttackCapicty{0}
{
	SetAttackCircleRadius(10);
	SetApproachCircleRadius(20);
	SetGridCapacity(4);
}

void KungFuGrid::SetGridCapacity(int capacity)
{
	int difference{capacity - m_MaxedGridCapacity };
	if (difference == 0)
	{
		return;
	}
	if (difference < 0) // smaller 
	{
		difference = abs(difference);
		m_Nodes.erase(m_Nodes.begin(), m_Nodes.begin() + difference);
	}
	else // bigger 
	{
		for (int i = 0; i < difference; i++)
		{
			m_Nodes.push_back(Node());
		}
	}
	m_MaxedGridCapacity = capacity;
	UpdateNodePositions();
}
void KungFuGrid::Update(float deltaTime, const Elite::Vector2& middlePos)
{
	m_MiddlePos = middlePos;
	UpdateNodePositions();
}

void KungFuGrid::Render(float deltaTime) const
{
	Elite::Color attackRadiusColor{1,0,0,1};
	const Elite::Color approachRadiusColor{0,0,1,1};
	const Elite::Color waitingRadiusColor{0,1,0,1};

	DEBUGRENDERER2D->DrawCircle(m_MiddlePos, m_AttackCircleRadius, attackRadiusColor, 0);
	DEBUGRENDERER2D->DrawCircle(m_MiddlePos, m_ApproachCircleRadius, approachRadiusColor, 0);
	DEBUGRENDERER2D->DrawCircle(m_MiddlePos, m_WaitingCircleRadius, waitingRadiusColor, 0);

	const int size = m_Nodes.size();
	const float anglePerNode = 2 * float(M_PI) / float(m_MaxedGridCapacity);
	const float angleOffset = anglePerNode / 2.f;

	for (int i = 0; i < size; i++)
	{
		DEBUGRENDERER2D->DrawPoint(m_Nodes[i].pos, 3, Elite::Color{ 0,0,0,1 }, 0);
		Elite::Vector2 secondPos{};
		secondPos.x = m_MiddlePos.x + m_ApproachCircleRadius * cosf(i * anglePerNode + angleOffset);
		secondPos.y = m_MiddlePos.y + m_ApproachCircleRadius * sinf(i * anglePerNode + angleOffset);
		Elite::Vector2 direction = secondPos - m_MiddlePos; 
		const float length = direction.Normalize();
		DEBUGRENDERER2D->DrawDirection(m_MiddlePos, direction, length, Elite::Color{ 0,0,0,1 },0);
	}
}

bool KungFuGrid::RequestAccesToApproachCircle(Creature* pCreature, int nodeIndex)
{
	// checking if it's in the waiting circle
	const float squaredDistance = Elite::DistanceSquared(pCreature->GetPosition(), m_MiddlePos);
	if (Elite::Square(m_WaitingCircleRadius) < squaredDistance)
	{
		return false;
	}

	pCreature->GetGridWeight();

	//// let them attack 
	//std::vector<Attack> possibleAttacks;
	//for (const Attack& attack : pCreature->GetAttacks())
	//{
	//	if (attack.AttackWeight + m_CurrentAttackCapicty <= m_MaxAttackCapacity)
	//	{
	//		possibleAttacks.push_back(attack);
	//	}
	//}
	//if (possibleAttacks.empty())
	//{
	//	return false;
	//}
}

bool KungFuGrid::GetPositionFromNodeIndex(int nodeIndex, Elite::Vector2& position)
{
	if (nodeIndex >= m_MaxedGridCapacity)
	{
		return false;
	}
	position = m_Nodes[nodeIndex].pos;
	return true;
}

void KungFuGrid::UpdateNodePositions()
{
	const float anglePerNode = 2 * float(M_PI) / float(m_MaxedGridCapacity);
	// the approachCircleRadius should always be bigger then the circleRadius
	const float differenceBetweenRadiuses = m_ApproachCircleRadius - m_AttackCircleRadius;
	const float radius = m_AttackCircleRadius + (differenceBetweenRadiuses / 2.f);
	const int size = m_Nodes.size();
	for (int i = 0; i < size; i++)
	{
		m_Nodes[i].pos.x = m_MiddlePos.x + radius * cosf(i * anglePerNode);
		m_Nodes[i].pos.y = m_MiddlePos.y + radius * sinf(i * anglePerNode);
	}
}