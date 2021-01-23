#include "stdafx.h"
#include "CombinedSteeringBehaviors.h"
#include <algorithm>
#include "../SteeringAgent.h"

BlendedSteering::BlendedSteering(vector<WeightedBehavior> weightedBehaviors)
	:m_WeightedBehaviors(weightedBehaviors)
{
};

//****************
//BLENDED STEERING
SteeringOutput BlendedSteering::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput blendedSteering = {};
	auto totalWeight = 0.f;

	for (auto weightedBehavior : m_WeightedBehaviors)
	{
		auto steering = weightedBehavior.pBehavior->CalculateSteering(deltaT, pAgent);
		if (!steering.IsValid)
		{
			continue;
		}
		blendedSteering.LinearVelocity += weightedBehavior.weight * steering.LinearVelocity;

		blendedSteering.AngularVelocity += weightedBehavior.weight * steering.AngularVelocity;

		totalWeight += weightedBehavior.weight;
	}

	if (totalWeight > 0.f)
	{
		auto scale = 1.f / totalWeight;
		blendedSteering.LinearVelocity *= scale;
		blendedSteering.AngularVelocity *= scale;
	}

	//if (pAgent->CanRenderBehavior())
	//	DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), blendedSteering.LinearVelocity, 7, { 0, 1, 1 }, 0.40f);
	return blendedSteering;
}

void BlendedSteering::SetWeightByEnum(SteeringBehavior eBehavior, float weight)
{
	for (WeightedBehavior& weightedBehavior : m_WeightedBehaviors)
	{
		if (weightedBehavior.eBehavior == eBehavior)
			weightedBehavior.weight = weight;
	}
}

ISteeringBehavior* BlendedSteering::GetBehaviorFromEnum(SteeringBehavior eBehavior)
{
	for (const WeightedBehavior& weightedBehavior : m_WeightedBehaviors)
	{
		if (weightedBehavior.eBehavior == eBehavior)
			return weightedBehavior.pBehavior;
	}
	return nullptr;
}

//*****************
//PRIORITY STEERING
SteeringOutput PrioritySteering::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	for (auto pBehavior : m_PriorityBehaviors)
	{
		steering = pBehavior->CalculateSteering(deltaT, pAgent);

		if (steering.IsValid)
			break;
	}

	//If non of the behavior return a valid output, last behavior is returned
	return steering;
}