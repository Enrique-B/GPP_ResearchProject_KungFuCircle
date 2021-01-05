/*=============================================================================*/
// Copyright 2019-2020
// Authors: Yosha Vandaele
/*=============================================================================*/
// SpacePartitioning.h: Contains Cell and Cellspace which are used to partition a space in segments.
// Cells contain pointers to all the agents within.
// These are used to avoid unnecessary distance comparisons to agents that are far away.

// Heavily based on chapter 3 of "Programming Game AI by Example" - Mat Buckland
/*=============================================================================*/

#pragma once
#include <list>
#include <vector>
#include <iterator>
#include "framework\EliteMath\EVector2.h"
#include "framework\EliteGeometry\EGeometry2DTypes.h"

class SteeringAgent;

// --- Cell ---
// ------------
struct Cell
{
	Cell(float left, float bottom, float width, float height);

	std::vector<Elite::Vector2> GetRectPoints() const;
	
	// all the agents currently in this cell
	std::list<SteeringAgent*> agents;
	Elite::Rect boundingBox;
};

// --- Partitioned Space ---
// -------------------------
class CellSpace
{
public:
	CellSpace(float width, float height, int rows, int cols, int maxEntities);

	void AddAgent(SteeringAgent* agent);
	void UpdateAgentCell(SteeringAgent* agent, const Elite::Vector2& oldPos);

	void RenderNeighborCells(const Elite::Vector2& targetPos, float queryRadius)const;
	void RegisterNeighbors(SteeringAgent* pAgent, float queryRadius);

	const std::vector<SteeringAgent*>& GetNeighbors() const { return m_Neighbors; }
	int GetNrOfNeighbors() const { return m_NrOfNeighbors; }

	void RenderCells()const;
	void SetBruteForce(bool bruteForce) { m_BruteForce = bruteForce; }
	bool GetBruteForce() { return m_BruteForce; }
private:
	// Cells and properties
	std::vector<Cell> m_Cells;

	float m_SpaceWidth;
	float m_SpaceHeight;

	int m_NrOfRows;
	int m_NrOfCols;

	float m_CellWidth;
	float m_CellHeight;

	// Members to avoid memory allocation on every frame
	vector<SteeringAgent*> m_Neighbors;
	int m_NrOfNeighbors;

	bool m_BruteForce; 

	// Helper functions
	int PositionToIndex(const Elite::Vector2& pos) const;
	int PositionToIndexNew(Elite::Vector2 pos) const;
	void Initialize(); 
	void BruteForceRegisterNeighbors(SteeringAgent* pAgent, float queryRadius);
	void NonBruteForceRegisterNeighbors(SteeringAgent* pAgent, float queryRadius);
	void BruteForceRenderNeighborCells(const Elite::Vector2& pos, float queryRadius)const;
	void NonBruteForceRenderNeighborCells(const Elite::Vector2& pos, float queryRadius)const;
};
