#pragma once
#include "Creature.h" // need the include for the attack struct
#include <unordered_map>
class KungFuGrid;
class StageManager
{
public:
	StageManager(); 
	~StageManager();
	void SetPlayerKungFuGrid(KungFuGrid* pGrid);
	bool RequestAccesToApproachCircle(Creature* pCreature, int nodeIndex);
	bool RequestAttack(Creature* pCreature, Attack& attack);
	bool GetPositionFromNodeIndex(int nodeIndex, Elite::Vector2& position);
	bool IsPositionInOuterCircle(const Elite::Vector2& position);
	void RemoveCreatureFromGrid(Creature* pCreature);
	const Elite::Vector2& GetMiddlePoint()const;
private:
	KungFuGrid* m_pPlayerKungFuGrid; 
	int m_CurrentGridCapacity;
	int m_CurrentAttackCapicty;
	std::unordered_map<Creature*, Attack> m_pCreaturesOnGrid;
};

