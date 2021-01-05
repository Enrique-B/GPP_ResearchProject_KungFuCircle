//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "SteeringAgent.h"

//https://gamedevelopment.tutsplus.com/series/understanding-steering-behaviors--gamedev-12732
//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition(); // desiredvelocity 
	steering.LinearVelocity.Normalize(); // normalize velocity 
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0,0.5f }, 0.4f);
	return steering;
}

//WANDER (base> SEEK)
//******
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	// calculating the position of the cirlce 
	Elite::Vector2 circlePos = pAgent->GetPosition() + pAgent->GetLinearVelocity().GetNormalized() * m_Offset;
	// making sure the wander angle can also be negative 
	m_WanderAngle += randomFloat(-0.5f, 0.5f) * m_MaxAngleChange;
	// calculating the point on the circle (there might be a more optimized way of doing this with dot and cross product)
	Elite::Vector2 offsetFromCircle = Elite::Vector2{cos(m_WanderAngle) , sin(m_WanderAngle)} * m_Radius;
	// setting the target
	m_Target = TargetData(circlePos + offsetFromCircle);
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawCircle(circlePos, m_Radius, Elite::Color(1,0,0,1), 0.5f);
		DEBUGRENDERER2D->DrawCircle(m_Target.Position, m_Radius / 4.f, Elite::Color(0, 1, 0, 1), 0.4f);
	}
	return Seek::CalculateSteering(deltaT, pAgent);
}

SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	auto distanceToTarget = DistanceSquared(pAgent->GetPosition(), m_Target.Position); 
	SteeringOutput output;
	if (distanceToTarget > (m_fleeRadius * m_fleeRadius))
	{
		output.IsValid = false; 
		return output; 
	}
	output.LinearVelocity = pAgent->GetPosition() - m_Target.Position;
	output.LinearVelocity.Normalize();
	output.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), output.LinearVelocity, 5, { 0,1,0,0.5f }, 0.4f);
	return output;
}

SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
//https://stackoverflow.com/questions/21483999/using-atan2-to-find-angle-between-two-vectors#:~:text=10%20Answers&text=is%20the%20angle%20between%20the,problably%20not%20what%20you%20meant.&text=The%20proper%20way%20to%20do,with%20the%20Atan2()%20function
	pAgent->SetAutoOrient(false); 
	SteeringOutput output{};
	const Elite::Vector2 diffTargetPos = m_Target.Position - pAgent->GetPosition();
	// gettting the angle from 2 vectors and doing + 90 degrees because for some reason it has an offset of -90 degrees
	float angle = atan2(diffTargetPos.y, diffTargetPos.x) + float(M_PI/2.f);
	float neededRotation = angle - pAgent->GetOrientation();
	// checking if the angle is between -M_PI and M_PI 
	if (neededRotation > M_PI)
		neededRotation -= float(2 * M_PI);
	else if (neededRotation < -M_PI)
		neededRotation += float(2 * M_PI);

	output.AngularVelocity = pAgent->GetMaxAngularSpeed() * (neededRotation / float(M_PI / 2.f));
	return output;
}

SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput output{};

	Elite::Vector2 toTarget = m_Target.Position - pAgent->GetPosition();
	const float distance = toTarget.Magnitude();
	if (distance < m_ArrivalRadius)
	{
		output.LinearVelocity = Elite::ZeroVector2;
		return output;
	}
	toTarget.Normalize();
	if (distance < m_SlowRadius)
		toTarget *= pAgent->GetMaxAngularSpeed() * (distance / m_SlowRadius);
	else
		toTarget *= pAgent->GetMaxAngularSpeed();
	output.LinearVelocity = toTarget;
	return output;
}

//https://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-pursuit-and-evade--gamedev-2946
SteeringOutput Persuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	const float t = Distance(m_Target.Position, pAgent->GetPosition());
	// the t is the time to calculate in the future 
	// the longer the lenght is the longer in time we will have to calculate the position of where we can intersept the target
	SteeringOutput output{};
	output.LinearVelocity = (m_Target.Position + m_Target.LinearVelocity * t / pAgent->GetMaxLinearSpeed()) - (pAgent->GetPosition() );
	output.LinearVelocity.Normalize();
	output.LinearVelocity *= pAgent->GetMaxAngularSpeed();
	return output;
}

SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput output{};
	
	// the t is the time to calculate in the future 
	const float t = Distance(m_Target.Position, pAgent->GetPosition());
	if (t > m_EvadeRadius)
	{
		output.IsValid = false;
		return output;
	}
	// future position of the target
	// deviding the t with the max linear speed is so the distance is lowered, this is better if the 2 speeds are opposites of each other
	const Elite::Vector2 futurePos{ m_Target.Position + m_Target.LinearVelocity * t / pAgent->GetMaxLinearSpeed() };
	m_Target = TargetData(futurePos);
	if (pAgent->CanRenderBehavior())
	{
		// future position
		DEBUGRENDERER2D->DrawPoint(futurePos, 10, Elite::Color{ 0,1,0 });
		// evade radius 
		DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), m_EvadeRadius, Elite::Color{ 1,1,1 }, 0);
	}
	return Flee::CalculateSteering(deltaT, pAgent);
}
