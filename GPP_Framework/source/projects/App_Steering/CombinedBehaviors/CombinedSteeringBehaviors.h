#pragma once
#include "../SteeringBehaviors.h"

class Flock;
enum class SteeringBehavior
{
	None, Seek, Wander, Flee, Arrive, Face, Persuit, Evade, Stand
};

//****************
//BLENDED STEERING
class BlendedSteering final : public ISteeringBehavior
{
public:
	struct WeightedBehavior
	{
		ISteeringBehavior* pBehavior = nullptr;
		float weight = 0.f;
		SteeringBehavior eBehavior = SteeringBehavior::None;

		WeightedBehavior(ISteeringBehavior* pBehavior, SteeringBehavior behavior, float weight) :
			pBehavior(pBehavior),
			eBehavior(behavior),
			weight(weight)
		{};
	};
public:
	BlendedSteering(std::vector<WeightedBehavior> weightedBehaviors);

	void AddBehaviour(WeightedBehavior weightedBehavior) { m_WeightedBehaviors.push_back(weightedBehavior); }
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
	void SetWeightByEnum(SteeringBehavior eBehavior, float weight);
	ISteeringBehavior* GetBehaviorFromEnum(SteeringBehavior eBehavior);

private:
	friend class Flock;
	friend class App_CombinedSteering;
	std::vector<WeightedBehavior> m_WeightedBehaviors = {};
};

//*****************
//PRIORITY STEERING
class PrioritySteering final: public ISteeringBehavior
{
public:
	PrioritySteering(vector<ISteeringBehavior*> priorityBehaviors) 
		:m_PriorityBehaviors(priorityBehaviors) 
	{}

	void AddBehaviour(ISteeringBehavior* pBehavior) { m_PriorityBehaviors.push_back(pBehavior); }
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

private:
	vector<ISteeringBehavior*> m_PriorityBehaviors = {};
};