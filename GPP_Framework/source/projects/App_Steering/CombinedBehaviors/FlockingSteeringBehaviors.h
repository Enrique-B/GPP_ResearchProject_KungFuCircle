#pragma once
#include "../SteeringBehaviors.h"
class Flock; 

//SEPARATION - FLOCKING
//*********************
class Separation final : public Seek
{
public:
	Separation(Flock* pFlock) : m_pFlock{ pFlock } {};
	virtual ~Separation() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent * pAgent) override;
private: 
	Flock* m_pFlock;
};

//COHESION - FLOCKING
//*******************
class Cohesion final : public Seek
{
public:
	Cohesion(Flock* pFlock) : m_pFlock{ pFlock } {};
	virtual ~Cohesion() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent * pAgent) override;
private:
	Flock* m_pFlock;

};


//VELOCITY MATCH - FLOCKING
//************************
class Alignment final : public ISteeringBehavior
{
public:
	Alignment(Flock* pFlock) : m_pFlock{ pFlock } {};
	virtual ~Alignment() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent * pAgent) override;
private:
	Flock* m_pFlock;

};
