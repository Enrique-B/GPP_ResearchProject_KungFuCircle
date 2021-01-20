#include "stdafx.h"
#include "App_KungFuCircle.h"
#include "Creature.h"
#include "Player.h"
#include "StageManager.h"

App_KungFuCircle::~App_KungFuCircle()
{
	SAFE_DELETE(m_pPlayer);
	SAFE_DELETE(m_KungFuGrid);
	SAFE_DELETE(m_pStageManager);
	for (Creature* pCreature : m_pEnemyCreatures)
	{
		SAFE_DELETE(pCreature);
	}
}

void App_KungFuCircle::Start()
{
	m_KungFuGrid = new KungFuGrid{};
	m_pStageManager = new StageManager{};
	m_KungFuGrid->SetApproachCircleRadius(6); 
	m_KungFuGrid->SetAttackCircleRadius(3);
	m_KungFuGrid->SetGridCapacity(10);
	m_pPlayer = new Player{};

	m_pStageManager->SetPlayerKungFuGrid(m_KungFuGrid);
	int amountOfCreatures{1};
	m_pEnemyCreatures.reserve(amountOfCreatures);

	for (int i = 0; i < amountOfCreatures; i++)
	{
		m_pEnemyCreatures.push_back(new Creature{});
		m_pEnemyCreatures[i]->AddStageManager(m_pStageManager);
		float radius = m_KungFuGrid->GetWaitingCircleRadius();
		m_pEnemyCreatures[i]->SetPosition({ Elite::randomFloat(-radius,radius), Elite::randomFloat(-radius,radius) });
	}
}

void App_KungFuCircle::Update(float deltaTime)
{
	m_pPlayer->Update(deltaTime);
	for (Creature* pCreature : m_pEnemyCreatures)
	{
		pCreature->Update(deltaTime);
	}
	IMGUIUpdate();
	m_KungFuGrid->Update(deltaTime, m_pPlayer->GetPosition());

}

void App_KungFuCircle::Render(float deltaTime) const
{
	m_pPlayer->Render(deltaTime);
	for (Creature* pCreature : m_pEnemyCreatures)
	{
		pCreature->Render(deltaTime);
	}
	m_KungFuGrid->Render(deltaTime);
}

void App_KungFuCircle::IMGUIUpdate()
{
	// Code From teachers

	//Setup
	int menuWidth = 235;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
	ImGui::Begin("Gameplay Programming KungFuCircle", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
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

	ImGui::Text("KungFuCircle");
	ImGui::Spacing();
	float approachRadius = m_KungFuGrid->GetApproachCircleRadius();
	float attackRadius = m_KungFuGrid->GetAttackCircleRadius();
	int gridCapacity = m_KungFuGrid->GetGridCapacity();

	ImGui::SliderFloat("approachRadius", &approachRadius, 0, 40, "%5");
	ImGui::SliderFloat("attackRadius", &attackRadius, 0, 40, "%5");
	ImGui::SliderInt("gridCapacity", &gridCapacity, 2, 20, "%1");

	m_KungFuGrid->SetApproachCircleRadius(approachRadius);
	m_KungFuGrid->SetAttackCircleRadius(attackRadius);
	m_KungFuGrid->SetGridCapacity(gridCapacity);
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
}
