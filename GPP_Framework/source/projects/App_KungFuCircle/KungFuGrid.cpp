#include "stdafx.h"
#include "KungFuGrid.h"
#include "Creature.h"

KungFuGrid::KungFuGrid()
	:m_MiddlePos{}
{
	for (size_t i = 0; i < 8; i++)
	{
		m_Nodes.push_back(Node(i));
	}

	SetAttackCircleRadius(10);
	SetApproachCircleRadius(20);
	SetGridCapacity(4);
	SetAttackCapacity(10);
}

void KungFuGrid::SetGridCapacity(int capacity)
{
	m_GridCapacity = capacity;
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
	const float anglePerNode = 2 * float(M_PI) / float(size);
	const float angleOffset = anglePerNode / 2.f;
	for (int i = 0; i < size; i++)
	{
		if (m_Nodes[i].isOccupied)
		{
			DEBUGRENDERER2D->DrawPoint(m_Nodes[i].pos, 3, Elite::Color{ 1,0,0,1 }, 0);
		}
		else
		{
			DEBUGRENDERER2D->DrawPoint(m_Nodes[i].pos, 3, Elite::Color{ 0,0,0,1 }, 0);
		}
		DEBUGRENDERER2D->DrawPoint(m_Nodes[i].pos, 3, Elite::Color{ 0,0,0,1 }, 0);
		Elite::Vector2 secondPos{};
		secondPos.x = m_MiddlePos.x + m_ApproachCircleRadius * cosf(i * anglePerNode + angleOffset);
		secondPos.y = m_MiddlePos.y + m_ApproachCircleRadius * sinf(i * anglePerNode + angleOffset);
		Elite::Vector2 direction = secondPos - m_MiddlePos; 
		const float length = direction.Normalize();
		DEBUGRENDERER2D->DrawDirection(m_MiddlePos, direction, length, Elite::Color{ 0,0,0,1 },0);
	}
}

bool KungFuGrid::GetClosestNode(const Elite::Vector2& enemyPos, int& nodeIndex)
{
	std::vector<Node> availableNodes; 
	for (const Node& node : m_Nodes)
	{
		if (!node.isOccupied)
		{
			availableNodes.push_back(node);
		}
	}
	if (availableNodes.empty())
	{
		return false;
	}

	Node closestNode{-1};
	float closestDistance = FLT_MAX;
	for (const Node& node : availableNodes)
	{
		const float distance = Elite::DistanceSquared(node.pos, enemyPos);
		if (distance < closestDistance)
		{
			closestDistance = distance; 
			closestNode = node;
		}
	}
	m_Nodes[closestNode.index].isOccupied = true;
	std::cout << "Node " << closestNode.index << " is occupied \n";
	nodeIndex = closestNode.index;
	return true;
}

bool KungFuGrid::GetPositionFromNodeIndex(int nodeIndex, Elite::Vector2& position)
{
	if (nodeIndex >= m_Nodes.size())
	{
		return false;
	}
	position = m_Nodes[nodeIndex].pos;
	return true;
}

void KungFuGrid::SetNodeUnoccupied(int nodeIndex)
{
	if (static_cast<unsigned int>(nodeIndex) >= m_Nodes.size())
	{
		return;
	}
	std::cout << "Node " << nodeIndex << " is not occupied anymore\n";
	m_Nodes[nodeIndex].isOccupied = false;
}

void KungFuGrid::UpdateNodePositions()
{
	const int size = m_Nodes.size();
	const float anglePerNode = 2 * float(M_PI) / float(size);
	// the approachCircleRadius should always be bigger then the circleRadius
	const float differenceBetweenRadiuses = m_ApproachCircleRadius - m_AttackCircleRadius;
	const float radius = m_AttackCircleRadius + (differenceBetweenRadiuses / 2.f);
	for (int i = 0; i < size; i++)
	{
		m_Nodes[i].pos.x = m_MiddlePos.x + radius * cosf(i * anglePerNode);
		m_Nodes[i].pos.y = m_MiddlePos.y + radius * sinf(i * anglePerNode);
	}
}