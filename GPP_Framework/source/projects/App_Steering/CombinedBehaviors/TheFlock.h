#pragma once
#include "../SteeringHelpers.h"
#include "FlockingSteeringBehaviors.h"

class ISteeringBehavior;
class SteeringAgent;
class BlendedSteering;
class PrioritySteering;
class CellSpace;

class Flock
{
public:
	Flock(
		int flockSize = 50, 
		float worldSize = 100.f, 
		SteeringAgent* pAgentToEvade = nullptr, 
		bool trimWorld = false);

	~Flock();

	void Update(float deltaT);
	void UpdateSeekTarget(const TargetData& target);
	void UpdateAndRenderUI();
	void Render(float deltaT);

	void RegisterNeighbors(SteeringAgent* pAgent);
	int GetNrOfNeighbors() const { return m_NrOfNeighbors; }
	const vector<SteeringAgent*>& GetNeighbors() const { return m_Neighbors; }

	Elite::Vector2 GetAverageNeighborPos() const;
	Elite::Vector2 GetAverageSeparationDirection(const Elite::Vector2& pos)const;
	Elite::Vector2 GetAverageNeighborVelocity() const;

private:
	// flock agents
	int m_FlockSize = 0;
	vector<SteeringAgent*> m_Agents;
	std::vector<Elite::Vector2>m_AgentPos;

	// neighborhood agents
	vector<SteeringAgent*> m_Neighbors;
	float m_NeighborhoodRadius = 5;
	int m_NrOfNeighbors = 0;

	// evade target
	SteeringAgent* m_pAgentToEvade = nullptr;

	// world info
	bool m_TrimWorld = false;
	float m_WorldSize = 0.f;

	// spatial partitioning
	bool m_Partition = true;
	bool m_DebugPartition = false;
	CellSpace* m_pCellspace = nullptr;
	
	// steering Behaviors
	BlendedSteering* m_pBlendedSteering = nullptr;
	PrioritySteering* m_pPrioritySteering = nullptr;

	Separation* m_pSeparation; 
	Cohesion* m_pCohesion; 
	Alignment* m_pAlignment; 
	Seek* m_pSeek; 
	Wander* m_pWander;
	Evade* m_pEvade;
	// ImGui info 
	bool m_DebugRenderNeighborhood;
	bool m_DebugRenderSteering;

	// private functions
	float* GetWeight(ISteeringBehavior* pBehaviour);
	void Initialize();
	void RenderDebugFromUI();
private:
	Flock(const Flock& other);
	Flock& operator=(const Flock& other);
};