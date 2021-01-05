#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "TheFlock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"

//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput output;
	// check if i have Neighbors 
	if (m_pFlock->GetNrOfNeighbors() == 0)
		return output;
	// GetAverageSeparationDirection is normalized
	output.LinearVelocity = m_pFlock->GetAverageSeparationDirection(pAgent->GetPosition());
	output.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	return output;
}

//*******************
//COHESION (FLOCKING)

SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput output; 
	// check if i have Neighbors 
	if (m_pFlock->GetNrOfNeighbors() == 0)
		return output;
	const Elite::Vector2 averagePos(m_pFlock->GetAverageNeighborPos());
	output.LinearVelocity = averagePos - pAgent->GetPosition();
	output.LinearVelocity.Normalize();
	output.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawPoint(averagePos, 3, Elite::Color{ 1,1,1,1 });
	}

	return output;
}

//*************************
//VELOCITY MATCH (FLOCKING)

SteeringOutput Alignment::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput output; 
	// check if i have Neighbors 
	if (m_pFlock->GetNrOfNeighbors() == 0)
		return output;

	// get the neighberhood velicty so i have the direction and make it go faster
	output.LinearVelocity = m_pFlock->GetAverageNeighborVelocity();
	output.LinearVelocity.Normalize();
	output.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	return output;
}
