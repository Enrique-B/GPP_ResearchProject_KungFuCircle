#pragma once
#include "projects/Shared/BaseAgent.h"
class Player :public BaseAgent
{
public:
	Player();
	virtual void Update(float dt)override;
	virtual void Render(float dt)override;
private:

};

