#include "stdafx.h"
#include "TheFlock.h"

#include "../SteeringAgent.h"
#include "../SteeringBehaviors.h"
#include "CombinedSteeringBehaviors.h"
#include "SpacePartitioning.h"
using namespace Elite;

//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/, 
	float worldSize /*= 100.f*/, 
	SteeringAgent* pAgentToEvade /*= nullptr*/, 
	bool trimWorld /*= false*/)

	: m_WorldSize{ worldSize }
	, m_FlockSize{ flockSize }
	, m_TrimWorld { trimWorld }
	, m_pAgentToEvade{pAgentToEvade}
	, m_NeighborhoodRadius{ 15 }
	, m_NrOfNeighbors{0}
{
	Initialize();
}

Flock::~Flock()
{
	for (int i = 0; i < m_FlockSize; i++)
	{
		SAFE_DELETE(m_Agents[i]);
	}
	SAFE_DELETE(m_pBlendedSteering); 
	SAFE_DELETE(m_pPrioritySteering);
	SAFE_DELETE(m_pSeparation);
	SAFE_DELETE(m_pCohesion);
	SAFE_DELETE(m_pAlignment);
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pEvade);
	SAFE_DELETE(m_pCellspace);
}

void Flock::Initialize()
{
	m_pCellspace = new CellSpace{m_WorldSize * 2.f, m_WorldSize * 2.f, 25, 25, m_FlockSize};

	m_pSeparation = new Separation(this);
	m_pCohesion = new Cohesion(this);
	m_pAlignment = new Alignment(this);
	m_pSeek = new Seek();
	m_pWander = new Wander();
	m_pEvade = new Evade();
	m_pEvade->SetTarget(TargetData(m_pAgentToEvade->GetPosition(), m_pAgentToEvade->GetOrientation(), m_pAgentToEvade->GetLinearVelocity(), m_pAgentToEvade->GetAngularVelocity()));

	m_pBlendedSteering = new BlendedSteering{ { {m_pSeparation, 1 / 5.f}, {m_pCohesion,1 / 5.f}, {m_pAlignment,1 / 5.f}, {m_pSeek, 1 / 5.f} ,{m_pWander, 1 / 5.f} } };
	m_pPrioritySteering = new PrioritySteering{ {m_pEvade, m_pBlendedSteering} };
	m_Neighbors.resize(m_FlockSize);
	m_Agents.reserve(m_FlockSize);
	m_AgentPos.reserve(m_FlockSize);
	Elite::Vector2 randomPos{};
	for (int i = 0; i < m_FlockSize; i++)
	{
		SteeringAgent* pAgent = new SteeringAgent{};
		pAgent->SetAutoOrient(true);
		pAgent->SetSteeringBehavior(m_pPrioritySteering);
		randomPos = randomVector2(m_WorldSize);
		pAgent->SetPosition(randomPos);
		m_AgentPos.push_back(randomPos);
		m_Agents.push_back(pAgent);
		m_pCellspace->AddAgent(pAgent);
	}
}

void Flock::Update(float deltaT)
{
	// update the seek target
	m_pEvade->SetTarget(TargetData(m_pAgentToEvade->GetPosition(), m_pAgentToEvade->GetOrientation(), m_pAgentToEvade->GetLinearVelocity(), m_pAgentToEvade->GetAngularVelocity()));
	
	// loop over all the boids
	for (int i = 0; i < m_FlockSize; i++)
	{
		// register its neighbors
		RegisterNeighbors(m_Agents[i]);
		// update it
		m_Agents[i]->Update(deltaT);
		// trim it to the world
		if (m_TrimWorld)
			m_Agents[i]->TrimToWorld(m_WorldSize);
		if (m_Partition)
		{
			m_pCellspace->UpdateAgentCell(m_Agents[i], m_AgentPos[i]);
			m_AgentPos[i] = m_Agents[i]->GetPosition();
		}
	}
}

void Flock::UpdateSeekTarget(const TargetData& target)
{
	m_pSeek->SetTarget(target);
}

void Flock::Render(float deltaT)
{
#if _DEBUG
	for (const auto& agent : m_Agents)
		agent->Render(deltaT);
#endif // _DEBUG

	RenderDebugFromUI();
	if (m_Partition && m_DebugPartition)
		m_pCellspace->RenderCells();
}

void Flock::RenderDebugFromUI()
{
	for (int i = 0; i < m_FlockSize; i++)
	{
		// color's aren't being rendered in release so i don't have to reset the color 
#if _DEBUG
		m_Agents[i]->SetBodyColor(Elite::Color(1, 1, 0));
#endif
		// but this is neccesary 
		m_Agents[i]->SetRenderBehavior(m_DebugRenderSteering);
	}

	if (m_DebugRenderNeighborhood)
	{
		if (m_Partition && m_DebugPartition)
			m_pCellspace->RenderNeighborCells(m_Agents[0]->GetPosition(), m_NeighborhoodRadius);
		// color's aren't being rendered in release so i don't have to do this 
#if _DEBUG
		RegisterNeighbors(m_Agents[0]);
		// collor the neighbors a different color 
		for (int i = 0; i < m_NrOfNeighbors; i++)
			m_Neighbors[i]->SetBodyColor(Elite::Color(0, 1, 1));
		DEBUGRENDERER2D->DrawCircle(m_Agents[0]->GetPosition(), m_NeighborhoodRadius, Elite::Color(0, 0, 1), 0);
#endif
		m_Agents[0]->SetRenderBehavior(true);
	}
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);

	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Text("LMB: place target");
	ImGui::Text("RMB: move cam.");
	ImGui::Text("Scrollwheel: zoom cam.");
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Flocking");
	ImGui::Spacing();

	// Implement checkboxes and sliders here
	ImGui::Text("Behavior Weights");
	ImGui::Spacing();
	ImGui::SliderFloat("Separation", GetWeight(m_pSeparation), 0.f, 1.f, "%.2f");
	ImGui::SliderFloat("Cohesian", GetWeight(m_pCohesion), 0.f, 1.f, "%.2f");
	ImGui::SliderFloat("Alignment", GetWeight(m_pAlignment), 0.f, 1.f, "%.2f");
	ImGui::SliderFloat("Seek", GetWeight(m_pSeek), 0.f, 1.f, "%.2f");
	ImGui::SliderFloat("Wander", GetWeight(m_pWander), 0.f, 1.f, "%.2f");

	ImGui::Checkbox("Debug Render steering", &m_DebugRenderSteering);
	ImGui::Checkbox("Debug Render neighborhood", &m_DebugRenderNeighborhood);
	ImGui::Checkbox("Partition world", &m_Partition);
	ImGui::Checkbox("Partition Debug", &m_DebugPartition);
	bool bruteForce{m_pCellspace->GetBruteForce()};
	ImGui::Checkbox("Partition BruteForce", &bruteForce);
	m_pCellspace->SetBruteForce(bruteForce);
	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
}

void Flock::RegisterNeighbors(SteeringAgent* pAgent)
{
	if (m_Partition)
	{
		m_pCellspace->RegisterNeighbors(pAgent, m_NeighborhoodRadius);
		m_Neighbors = m_pCellspace->GetNeighbors();
		m_NrOfNeighbors = m_pCellspace->GetNrOfNeighbors();
	}
	else
	{
		// register the agents neighboring the currently evaluated agent
		// store how many they are, so you know which part of the vector to loop over
		m_NrOfNeighbors = 0;
		// change this to memory pool 
		const float radiusSquared{ Square(m_NeighborhoodRadius) };
		const Elite::Vector2 thisAgentPosition{ pAgent->GetPosition() };
		for (SteeringAgent* pAgent2 : m_Agents)
		{
			if (pAgent2 != pAgent)
			{
				// checking if the distance is smaller/equal than the radius of the circle to know if it's in the circle
				if (DistanceSquared(pAgent2->GetPosition(), thisAgentPosition) <= radiusSquared)
				{
					// change this with memory pool implementation
					m_Neighbors[m_NrOfNeighbors] = pAgent2;
					m_NrOfNeighbors++;
				}
			}
		}
	}
}

Elite::Vector2 Flock::GetAverageNeighborPos() const
{
	Elite::Vector2 returnValue{};
	if (m_NrOfNeighbors <= 0)
		return returnValue;

	for (int i = 0; i < m_NrOfNeighbors; i++)
		returnValue += m_Neighbors[i]->GetPosition();

	return returnValue / float(m_NrOfNeighbors);
}

Elite::Vector2 Flock::GetAverageSeparationDirection(const Elite::Vector2& pos) const
{
	Elite::Vector2 separationDirection{};
	if (m_NrOfNeighbors <= 0)
		return separationDirection;

	for (int i = 0; i < m_NrOfNeighbors; i++)
	{
		// get the vector between myself and the neighbor 
		// then get the direction vector with the lenght
		// get the (radius - the length) * direction = separation influence of the neighbor
		// add the separation influence vector to the separation position
		Elite::Vector2 sepDirection = pos - m_Neighbors[i]->GetPosition();
		float neighborDistance = sepDirection.Normalize();
		separationDirection += (m_NeighborhoodRadius - neighborDistance) * sepDirection;
	} 
	return separationDirection.GetNormalized();
}

Elite::Vector2 Flock::GetAverageNeighborVelocity() const
{
	Elite::Vector2 returnValue{};
	if (m_NrOfNeighbors <= 0)
		return returnValue;

	for (int i = 0; i < m_NrOfNeighbors; i++)
		returnValue += m_Neighbors[i]->GetLinearVelocity();

	return returnValue / float(m_NrOfNeighbors);
}

float* Flock::GetWeight(ISteeringBehavior* pBehavior) 
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->m_WeightedBehaviors;
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if(it!= weightedBehaviors.end())
			return &it->weight;
	}
	return nullptr;
}