#pragma once

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class AStar
	{
	public:
		AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			T_NodeType* pNode = nullptr;
			T_ConnectionType* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);

	private:
		float GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
		Heuristic m_HeuristicFunction;
	};

	template <class T_NodeType, class T_ConnectionType>
	AStar<T_NodeType, T_ConnectionType>::AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction)
		: m_pGraph(pGraph)
		, m_HeuristicFunction(hFunction)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> AStar<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pGoalNode)
	{
		// Create a startRecord and add it to the openList to start the while loop 
		vector<T_NodeType*> path; 
		vector<NodeRecord> openList; 
		vector<NodeRecord> closedList; 
		NodeRecord currentRecord;
		NodeRecord startRecord; 
		startRecord.pNode = pStartNode; 
		startRecord.pConnection = nullptr;
		startRecord.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode);
		openList.push_back(startRecord);

		while (!openList.empty())
		{
			// get connection with lowest F score
			currentRecord = *std::min_element(openList.cbegin(), openList.cend());
			// check if that connection leads to the end node
			if (currentRecord.pNode == pGoalNode)
					break;

			if (currentRecord.pNode != nullptr)
			{
				// Else, we get all the connctions of the connection's end node 
				std::list<T_ConnectionType*> connections = m_pGraph->GetNodeConnections(currentRecord.pNode->GetIndex());
				for (T_ConnectionType* connection : connections)
				{
					auto nextnode = m_pGraph->GetNode(connection->GetTo());
					auto isNodeInNodeRecordLambda = [nextnode](auto nodeRecord) {return nodeRecord.pNode == nextnode; };
					// calculate the total cost so far
					float gCost = currentRecord.costSoFar + connection->GetCost();
					// check if any of the connections lead to a node already in the closed list 

					vector<NodeRecord>::const_iterator it = std::find_if(closedList.cbegin(), closedList.cend(), isNodeInNodeRecordLambda);
					if (it != closedList.cend())
					{
						// check if the already existing connection is cheaper 
						if ((*it).costSoFar <= gCost)
							continue;
						else
							closedList.erase(it);
					}
					// check if any of those connections lead to a node already in the open list
					else
					{
						it = std::find_if(openList.cbegin(), openList.cend(), isNodeInNodeRecordLambda);
						if (it != openList.cend())
						{
							// check if the already existing connection is cheaper 
							if ((*it).costSoFar <= gCost)
								continue;
							else
								openList.erase(it);
						}
					}
					// at this point any expensive conection should be removed 
					NodeRecord temp;
					temp.pConnection = connection;
					temp.pNode = m_pGraph->GetNode(connection->GetTo());
					temp.costSoFar = gCost;
					temp.estimatedTotalCost = gCost + GetHeuristicCost(temp.pNode, pGoalNode); // calculating f cost
					openList.push_back(temp);
				}
				// remove nodeRecord from the openlist and add it to the closed list 
				openList.erase(std::remove(openList.begin(), openList.end(), currentRecord));
				closedList.push_back(currentRecord);
			}
		}
		std::vector<T_NodeType*> pPath;
		// reconstruct path from last connection from the openlist and add it to the closedlist 
		while (currentRecord.pNode != pStartNode)
		{
			pPath.push_back(currentRecord.pNode);
			for (const auto& item : closedList)
			{
				if (item.pNode == m_pGraph->GetNode(currentRecord.pConnection->GetFrom()))
				{
					currentRecord = item;
					break;
				}
			}
		}
		pPath.push_back(pStartNode);
		std::reverse(pPath.begin(), pPath.end());
		std::cout << "index startNode = " << pStartNode->GetIndex() << std::endl;
		std::cout << "openList's size = " << openList.size() << std::endl;
		std::cout << "closedList's size = " << closedList.size() << std::endl;
		std::cout << "path size = " << pPath.size() << std::endl;
		return pPath;
	}

	template <class T_NodeType, class T_ConnectionType>
	float Elite::AStar<T_NodeType, T_ConnectionType>::GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const
	{
		Vector2 toDestination = m_pGraph->GetNodePos(pEndNode) - m_pGraph->GetNodePos(pStartNode);
		return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
	}
}