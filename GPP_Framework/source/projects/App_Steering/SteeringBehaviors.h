/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviors.h: SteeringBehaviors interface and different implementations
/*=============================================================================*/
#ifndef ELITE_STEERINGBEHAVIORS
#define ELITE_STEERINGBEHAVIORS

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "SteeringHelpers.h"
class SteeringAgent;
using namespace Elite;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) = 0;

	//Seek Functions
	void SetTarget(const TargetData& target) { m_Target = target; }

	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	TargetData m_Target;
};
#pragma endregion

class Stand : public ISteeringBehavior
{
public:
	Stand() = default;
	virtual ~Stand() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};


///////////////////////////////////////
//SEEK
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

//////////////////////////
//WANDER
//******
class Wander final : public Seek
{
public:
	Wander() = default;
	virtual ~Wander() = default;

	//Wander Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
	void SetWanderOffset(float offset) { m_Offset = offset; };
	void SetWanderRadius(float radius) { m_Radius = radius; };
	void SetMaxAngleChange(float rad) { m_MaxAngleChange = rad; }
private: 
	float m_Offset = 6.f; 
	float m_Radius = 4.f; 
	float m_MaxAngleChange = ToRadians(30);
	float m_WanderAngle = 0.f;
};

/////////////
//FLEE
//******
class Flee : public ISteeringBehavior
{
public:
	Flee() = default;
	virtual ~Flee() = default;

	//Wander Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
	void SetFleeRadius(float radius) { m_FleeRadius = radius; }
protected: 
	float m_FleeRadius{15.f};
};

/////////////
//Arrive
//******
class Arrive final : public ISteeringBehavior
{
public:
	Arrive() = default;
	virtual ~Arrive() = default;
	void SetArrivalRadius(float radius) { m_ArrivalRadius = radius; }
	void SetSlowRadius(float radius) { m_SlowRadius = radius; }
	//Wander Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private: 
	float m_ArrivalRadius = 10;
	float m_SlowRadius = 30;
};

/////////////
//Face
//******
class Face final : public ISteeringBehavior
{
public:
	Face() = default;
	virtual ~Face() = default;

	//Wander Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

/////////////
//Persuit
//******
class Persuit final : public ISteeringBehavior
{
public:
	Persuit() = default;
	virtual ~Persuit() = default;

	//Wander Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

/////////////
//Evade
//******
class Evade final : public Flee
{
public:
	Evade() = default;
	virtual ~Evade() = default;

	//Wander Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private: 
};

#endif