#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\App_Steering\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
	boundingBox.bottomLeft = { left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	auto left = boundingBox.bottomLeft.x;
	auto bottom = boundingBox.bottomLeft.y;
	auto width = boundingBox.width;
	auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_Neighbors(maxEntities)
	, m_NrOfNeighbors(0)
	, m_BruteForce{ false }
{
	Initialize();
}
void CellSpace::Initialize()
{
	m_CellWidth = m_SpaceWidth / m_NrOfRows;
	m_CellHeight = m_SpaceHeight / m_NrOfCols;
	m_Cells.reserve(m_NrOfRows * m_NrOfCols);
	for (int c = 0; c < m_NrOfCols; c++)
	{
		for (int r = 0; r < m_NrOfRows; r++)
		{
			// 0th element is bottom left
			m_Cells.push_back(Cell(-m_SpaceWidth /2.f + r * m_CellWidth, -m_SpaceHeight /2.f + c * m_CellHeight, m_CellWidth, m_CellHeight));
		}
	}
}

void CellSpace::BruteForceRegisterNeighbors(SteeringAgent* pAgent, float queryRadius)
{
	const Elite::Vector2 pos{ pAgent->GetPosition() };
	const Elite::Rect rect{ Elite::Vector2{pos.x - queryRadius, pos.y - queryRadius}, queryRadius * 2, queryRadius * 2 };
	const float radius2{ queryRadius * queryRadius };
	for (int c = 0; c < m_NrOfCols; c++)
	{
		for (int r = 0; r < m_NrOfRows; r++)
		{
			const int index{ c * m_NrOfCols + r };
			// got some overhead here 
			if (Elite::IsOverlapping(m_Cells[index].boundingBox, rect))
			{
				for (SteeringAgent* agent : m_Cells[index].agents)
				{
					if (pAgent != agent)
					{
						// looking if it's in the circle 
						if (DistanceSquared(agent->GetPosition(), pos) <= radius2)
						{
							m_Neighbors[m_NrOfNeighbors] = agent;
							m_NrOfNeighbors++;
						}
					}
				}
			}
		}
	}
}

void CellSpace::NonBruteForceRegisterNeighbors(SteeringAgent* pAgent, float queryRadius)
{
	const Elite::Vector2 pos{ pAgent->GetPosition() };
	const Elite::Rect rect{ Elite::Vector2{pos.x - queryRadius, pos.y - queryRadius}, queryRadius * 2, queryRadius * 2 };
	const float radius2{ queryRadius * queryRadius };

	// getting the max and min values for the clamp the - and + 1 is so it doens't clamp to the edge 
		// but one from it so the PositionToIndex function works better
	const float maxX = m_SpaceWidth / 2.f - 1;
	const float maxY = m_SpaceHeight / 2.f - 1;
	const float minX = -m_SpaceWidth / 2.f + 1;
	const float minY = -m_SpaceHeight / 2.f + 1;

	// clamping the rect around the circle so it doesn't go out of bounce 
	const float right = Elite::Clamp(rect.bottomLeft.x + rect.width, minX, maxX);
	const float top = Elite::Clamp(rect.bottomLeft.y + rect.height, minY, maxY);
	const float left = Elite::Clamp(rect.bottomLeft.x, minX, maxX);
	const float bottom = Elite::Clamp(rect.bottomLeft.y, minY, maxY);

	// don't need a bottom right because I can do it without 
	const Elite::Vector2 bottomLeft{ left,bottom };
	const Elite::Vector2 topRight{ right, top };
	const Elite::Vector2 topLeft{ left, top };

	// getting the indexes 
	const int bottomLeftIndex = PositionToIndex(bottomLeft);
	const int topRightIndex = PositionToIndex(topRight);
	const int topLeftIndex = PositionToIndex(topLeft);

	// find the differences in x and y positions 
	const int differenceX = topRightIndex - topLeftIndex;
	const int differenceY = (topLeftIndex - bottomLeftIndex) / m_NrOfRows;
	// loop over the cells
	for (int i = 0; i <= differenceY; i++)
	{
		for (int j = 0; j <= differenceX; j++)
		{
			const int index = bottomLeftIndex + j + (i * m_NrOfRows);
			for (SteeringAgent* agent : m_Cells[index].agents)
			{
				if (pAgent != agent)
				{
					// looking if it's in the circle 
					if (DistanceSquared(agent->GetPosition(), pos) <= radius2)
					{
						m_Neighbors[m_NrOfNeighbors] = agent;
						m_NrOfNeighbors++;
					}
				}
			}
		}
	}
}

void CellSpace::BruteForceRenderNeighborCells(const Elite::Vector2& pos, float queryRadius) const
{
	std::vector<Elite::Vector2> points;
	points.resize(4);
	const float radius2{ queryRadius * queryRadius };
	const Elite::Rect rect{ Elite::Vector2{pos.x - queryRadius, pos.y - queryRadius}, queryRadius * 2, queryRadius * 2 };
	points[0] = Elite::Vector2{ rect.bottomLeft.x,rect.bottomLeft.y + rect.height }; // topLeft
	points[1] = Elite::Vector2{ rect.bottomLeft.x + rect.width,rect.bottomLeft.y + rect.height }; // topRight
	points[2] = Elite::Vector2{ rect.bottomLeft.x + rect.width,rect.bottomLeft.y }; // bottomRight
	points[3] = Elite::Vector2{ rect.bottomLeft };// bottomLeft
	DEBUGRENDERER2D->DrawPolygon(&points[0], 4, Elite::Color{ 1,0,0, 0.5f }, 0.4f);

	for (int c = 0; c < m_NrOfCols; c++)
	{
		for (int r = 0; r < m_NrOfRows; r++)
		{
			const int index{ c * m_NrOfCols + r };
			if (Elite::IsOverlapping(m_Cells[index].boundingBox, rect))
			{
				// Color in holes 
				points[0] = Elite::Vector2{ m_Cells[index].boundingBox.bottomLeft.x,m_Cells[index].boundingBox.bottomLeft.y + m_Cells[index].boundingBox.height }; // topLeft
				points[1] = Elite::Vector2{ m_Cells[index].boundingBox.bottomLeft.x + m_Cells[index].boundingBox.width,m_Cells[index].boundingBox.bottomLeft.y + m_Cells[index].boundingBox.height }; // topRight
				points[2] = Elite::Vector2{ m_Cells[index].boundingBox.bottomLeft.x + m_Cells[index].boundingBox.width,m_Cells[index].boundingBox.bottomLeft.y }; // bottomRight
				points[3] = Elite::Vector2{ m_Cells[index].boundingBox.bottomLeft };// bottomLeft
				DEBUGRENDERER2D->DrawSolidPolygon(&points[0], 4, Elite::Color{ 1,0,0, 0.5f }, 0.4f);
			}
		}
	}
}

void CellSpace::NonBruteForceRenderNeighborCells(const Elite::Vector2& pos, float queryRadius)const
{
	std::vector<Elite::Vector2> points;
	points.resize(4);
	const float radius2{ queryRadius * queryRadius };
	const Elite::Rect rect{ Elite::Vector2{pos.x - queryRadius, pos.y - queryRadius}, queryRadius * 2, queryRadius * 2 };
	points[0] = Elite::Vector2{ rect.bottomLeft.x,rect.bottomLeft.y + rect.height }; // topLeft
	points[1] = Elite::Vector2{ rect.bottomLeft.x + rect.width,rect.bottomLeft.y + rect.height }; // topRight
	points[2] = Elite::Vector2{ rect.bottomLeft.x + rect.width,rect.bottomLeft.y }; // bottomRight
	points[3] = Elite::Vector2{ rect.bottomLeft };// bottomLeft
	DEBUGRENDERER2D->DrawPolygon(&points[0], 4, Elite::Color{ 1,0,0, 0.5f }, 0.4f);

	// getting the max and min values for the clamp the - and + 1 is so it doens't clamp to the edge 
		// but one from it so the PositionToIndex function works better
	const float maxX = m_SpaceWidth / 2.f - 1;
	const float maxY = m_SpaceHeight / 2.f - 1;
	const float minX = -m_SpaceWidth / 2.f + 1;
	const float minY = -m_SpaceHeight / 2.f + 1;
	// clamping the rect around the circle so it doesn't go out of bounce 
	const float right = Elite::Clamp(rect.bottomLeft.x + rect.width, minX, maxX);
	const float top = Elite::Clamp(rect.bottomLeft.y + rect.height, minY, maxY);
	const float left = Elite::Clamp(rect.bottomLeft.x, minX, maxX);
	const float bottom = Elite::Clamp(rect.bottomLeft.y, minY, maxY);

	const Elite::Vector2 bottomLeft{ left,bottom };
	const Elite::Vector2 bottomRight{ right, bottom };
	const Elite::Vector2 topRight{ right, top };
	const Elite::Vector2 topLeft{ left, top };
	// getting the indexes 
	// don't need a bottom right index because I can do it without 

	const int bottomLeftIndex = PositionToIndex(bottomLeft);
	const int topRightIndex = PositionToIndex(topRight);
	const int topLeftIndex = PositionToIndex(topLeft);

	// find the differences in x and y positions 
	const int differenceX = topRightIndex - topLeftIndex;
	const int differenceY = (topLeftIndex - bottomLeftIndex) / m_NrOfRows;

	DEBUGRENDERER2D->DrawSolidCircle(bottomLeft, 1, { 1,1 }, Elite::Color{ 1,1,1,1 });
	DEBUGRENDERER2D->DrawSolidCircle(bottomRight, 1, { 1,1 }, Elite::Color{ 1,1,1,1 });
	DEBUGRENDERER2D->DrawSolidCircle(topRight, 1, { 1,1 }, Elite::Color{ 1,1,1,1 });
	DEBUGRENDERER2D->DrawSolidCircle(topLeft, 1, { 1,1 }, Elite::Color{ 1,1,1,1 });

	// loop over the cells
	for (int i = 0; i <= differenceY; i++)
	{
		for (int j = 0; j <= differenceX; j++)
		{
			const int index = bottomLeftIndex + j + (i * m_NrOfRows);
			// Color in holes 
			points[0] = Elite::Vector2{ m_Cells[index].boundingBox.bottomLeft.x,m_Cells[index].boundingBox.bottomLeft.y + m_Cells[index].boundingBox.height }; // topLeft
			points[1] = Elite::Vector2{ m_Cells[index].boundingBox.bottomLeft.x + m_Cells[index].boundingBox.width,m_Cells[index].boundingBox.bottomLeft.y + m_Cells[index].boundingBox.height }; // topRight
			points[2] = Elite::Vector2{ m_Cells[index].boundingBox.bottomLeft.x + m_Cells[index].boundingBox.width,m_Cells[index].boundingBox.bottomLeft.y }; // bottomRight
			points[3] = Elite::Vector2{ m_Cells[index].boundingBox.bottomLeft };// bottomLeft
			DEBUGRENDERER2D->DrawSolidPolygon(&points[0], 4, Elite::Color{ 1,0,0, 0.5f }, 0.4f);
		}
	}

}

void CellSpace::AddAgent(SteeringAgent* agent)
{
	m_Cells[PositionToIndex(agent->GetPosition())].agents.push_back(agent);
}

void CellSpace::UpdateAgentCell(SteeringAgent* agent, const Elite::Vector2& oldPos)
{
	const Elite::Vector2 pos{agent->GetPosition()};
	const int oldIndex{ PositionToIndex(oldPos)};
	const int newIndex{ PositionToIndex(pos) };
	// checking if its not the same or the new index is bigger than the grid
	// the last one happens when it teleports from the bottem to top or the other way around
	if (oldIndex == newIndex)
		return;
	int size{ m_NrOfRows * m_NrOfCols }; // same thing as size but less expensive 
	if (oldIndex < size)
	{
		m_Cells[oldIndex].agents.remove(agent);
	}
	if (newIndex >= size)
		return;
	m_Cells[newIndex].agents.push_back(agent);
}


void CellSpace::RenderNeighborCells(const Elite::Vector2& pos, float queryRadius)const
{
	if (m_BruteForce)
		BruteForceRenderNeighborCells(pos, queryRadius);
	else
		NonBruteForceRenderNeighborCells(pos, queryRadius);
}

void CellSpace::RegisterNeighbors(SteeringAgent* pAgent, float queryRadius)
{
	m_NrOfNeighbors = 0;
	if (m_BruteForce)
		BruteForceRegisterNeighbors(pAgent, queryRadius);
	else
		NonBruteForceRegisterNeighbors(pAgent, queryRadius);
}

void CellSpace::RenderCells() const
{
	// using the same vector for the for loop makes it so i don't have to make a vector with 5 vectors every iteration of the for loop
	std::vector<Elite::Vector2> points;
	points.resize(4);
	for (const Cell&  cell : m_Cells)
	{
		points[0] = Elite::Vector2{cell.boundingBox.bottomLeft.x,cell.boundingBox.bottomLeft.y + cell.boundingBox.height }; // topLeft
		points[1] = Elite::Vector2{cell.boundingBox.bottomLeft.x + cell.boundingBox.width,cell.boundingBox.bottomLeft.y + cell.boundingBox.height }; // topRight
		points[2] = Elite::Vector2{cell.boundingBox.bottomLeft.x + cell.boundingBox.width,cell.boundingBox.bottomLeft.y }; // bottomRight
		points[3] = Elite::Vector2{cell.boundingBox.bottomLeft};// bottomLeft
		DEBUGRENDERER2D->DrawPolygon(&points[0], 4, Elite::Color{1,0,0}, 0.4f);

		DEBUGRENDERER2D->DrawString(points[0], std::to_string(cell.agents.size()).c_str());
		//DEBUGRENDERER2D->DrawString(points[0], std::to_string(index).c_str());
	}
}

int CellSpace::PositionToIndex(const Elite::Vector2& pos) const
{
	return Elite::Clamp(int((pos.x + m_SpaceWidth / 2.f) / m_CellWidth) + int((pos.y + m_SpaceHeight / 2.f) / m_CellHeight) * m_NrOfCols, 0, m_NrOfCols * m_NrOfRows);
}