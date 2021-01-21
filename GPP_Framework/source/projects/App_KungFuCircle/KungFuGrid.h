#pragma once

struct Node
{
	Node(int NodeIndex) : isOccupied{ false }, index{ NodeIndex }{}
	Elite::Vector2 pos;
	bool isOccupied; 
	int index;
}; 

class Creature; 
class KungFuGrid
{
public: 
	KungFuGrid();
	//getters and setters
	void SetAttackCircleRadius(float radius) { m_AttackCircleRadius = radius; };
	void SetApproachCircleRadius(float radius) {m_ApproachCircleRadius = radius; m_WaitingCircleRadius = radius + 5;}
	void SetGridCapacity(int Capacity);
	void SetAttackCapacity(int Capacity) { m_AttackCapacity = Capacity; }
	float GetApproachCircleRadius() const { return m_ApproachCircleRadius; }
	float GetAttackCircleRadius() const { return m_AttackCircleRadius; }
	float GetWaitingCircleRadius() const { return m_WaitingCircleRadius; }
	int GetGridCapacity()const { return m_GridCapacity; }
	int GetAttackCapacity()const { return m_AttackCapacity; }
	const Elite::Vector2& GetMiddlePos()const { return m_MiddlePos; };
	// other functions 
	void Update(float deltaTime, const Elite::Vector2& middlePos);
	void Render(float deltaTime)const;
	bool GetClosestNode(const Elite::Vector2& enemyPos, int& nodeIndex);
	bool GetPositionFromNodeIndex(int nodeIndex, Elite::Vector2& position);
	void SetNodeUnoccupied(int nodeIndex);
private: 
	Elite::Vector2 m_MiddlePos;
	std::vector<Node> m_Nodes;
	float m_WaitingCircleRadius;
	float m_ApproachCircleRadius; 
	float m_AttackCircleRadius;
	int m_GridCapacity; 
	int m_AttackCapacity; 
	void UpdateNodePositions();
};