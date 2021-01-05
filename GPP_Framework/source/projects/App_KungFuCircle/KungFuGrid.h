#pragma once

struct Node
{
	Elite::Vector2 pos;
	bool isOcupied; 
}; 

class Creature; 

class KungFuGrid
{
public: 
	KungFuGrid();
	void SetAttackCircleRadius(float radius) { m_AttackCircleRadius = radius; };
	void SetApproachCircleRadius(float radius) {m_ApproachCircleRadius = radius; m_WaitingCircleRadius = radius + 5;}
	void SetGridCapacity(int Capacity);
	void SetAttackCapacity(int Capacity) { m_MaxAttackCapacity = Capacity; }
	float GetApproachCircleRadius() const { return m_ApproachCircleRadius; }
	float GetAttackCircleRadius() const { return m_AttackCircleRadius; }
	int GetGridCapacity()const { return m_MaxedGridCapacity; }

	void Update(float deltaTime, const Elite::Vector2& middlePos);
	void Render(float deltaTime)const;

	bool RequestAccesToApproachCircle(Creature* pCreature,int nodeIndex);
	bool GetPositionFromNodeIndex(int nodeIndex, Elite::Vector2& position);
private: 
	Elite::Vector2 m_MiddlePos;
	std::vector<Node> m_Nodes;
	float m_WaitingCircleRadius;
	float m_ApproachCircleRadius; 
	float m_AttackCircleRadius;
	int m_MaxedGridCapacity; 
	int m_CurrentGridCapacity;
	int m_MaxAttackCapacity; 
	int m_CurrentAttackCapicty;
	void UpdateNodePositions();
};