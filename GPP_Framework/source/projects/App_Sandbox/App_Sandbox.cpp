//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_Sandbox.h"
#include "SandboxAgent.h"

//Destructor
App_Sandbox::~App_Sandbox()
{
	SAFE_DELETE(m_pSandboxAgent);
}

//Functions
void App_Sandbox::Start()
{
	m_pSandboxAgent = new SandboxAgent();
}

void App_Sandbox::Update(float deltaTime)
{
	if (INPUTMANAGER->IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{

	}

	m_pSandboxAgent->Update(deltaTime); 
}

void App_Sandbox::Render(float deltaTime) const
{
	m_pSandboxAgent->Render(deltaTime); 
}