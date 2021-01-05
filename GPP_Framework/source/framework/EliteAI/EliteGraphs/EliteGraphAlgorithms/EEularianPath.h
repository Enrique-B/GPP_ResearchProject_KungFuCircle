#pragma once
#include <stack>

namespace Elite
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	template <class T_NodeType, class T_ConnectionType>
	class EulerianPath
	{
	public:

		EulerianPath(IGraph<T_NodeType, T_ConnectionType>* pGraph);

		Eulerianity IsEulerian() const;
		vector<T_NodeType*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(int startIdx, vector<bool>& visited) const;
		bool IsConnected() const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
	};

	template<class T_NodeType, class T_ConnectionType>
	inline EulerianPath<T_NodeType, T_ConnectionType>::EulerianPath(IGraph<T_NodeType, T_ConnectionType>* pGraph)
		: m_pGraph(pGraph)
	{
	}

	template<class T_NodeType, class T_ConnectionType>
	inline Eulerianity EulerianPath<T_NodeType, T_ConnectionType>::IsEulerian() const
	{
		// If the graph is not connected, there can be no Eulerian Trail
		if (!IsConnected())
			return Eulerianity::notEulerian;

			// Count nodes with odd degree 
		int nrOfNodes = m_pGraph->GetNrOfNodes(); 
		int oddCount = 0;
		for (int i = 0; i < nrOfNodes; i++)
			if (m_pGraph->IsNodeValid(i) && m_pGraph->GetNodeConnections(i).size() & 1)
				oddCount++;

		// A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if (oddCount > 2)
			return Eulerianity::notEulerian;

		// A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (an Euler trail can be made, but only starting and ending in these 2 nodes)
		if (oddCount == 2 && nrOfNodes != 2)
			return Eulerianity::semiEulerian;

		// A connected graph with no odd nodes is Eulerian
		return Eulerianity::eulerian;
	}

	template<class T_NodeType, class T_ConnectionType>
	inline vector<T_NodeType*> EulerianPath<T_NodeType, T_ConnectionType>::FindPath(Eulerianity& eulerianity) const
	{
		auto path = vector<T_NodeType*>();
		if (!IsConnected())
		{
			eulerianity = Eulerianity::notEulerian;
			return path;
		}
		// Get a copy of the graph because this algorithm involves removing edges
		auto graphCopy = m_pGraph->Clone();
		int nrOfNodes = graphCopy->GetNrOfActiveNodes();
		vector<T_NodeType*> activeNodes = graphCopy->GetAllActiveNodes();
		T_NodeType* currentNode = nullptr;
		// checking what type of eulerian it is and capturing the odd nodes 
		int oddCount = 0;
		vector<T_NodeType*> oddVerticies;
		for (int i = 0; i < nrOfNodes; i++)
		{
			// looking if it's not even 
			size_t size = graphCopy->GetNodeConnections(activeNodes[i]->GetIndex()).size();
			if ((size % 2) != 0)
			{
				oddCount++;
				oddVerticies.push_back(activeNodes[i]);
			}
		}

		if (oddCount > 2)
		{
			eulerianity = Eulerianity::notEulerian;
			return path;

		}
		if (oddCount == 2 && nrOfNodes != 2)
		{
			eulerianity = Eulerianity::semiEulerian;
			currentNode = oddVerticies[0];
		}
		else
		{
			eulerianity = Eulerianity::eulerian;
			currentNode = activeNodes[0];
		}
		// algorithm...
		std::vector<T_NodeType*> stack;
		//Repeat step 2 until the current vertex has no more neighbors and the stack is empty.
		// stack needs an initial value 
		stack.push_back(nullptr);
		while (!stack.empty() || currentNode != nullptr)
		{
			//If current vertex has no neighbors - add it to circuit, 
			//remove the last vertex from the stack and set it as the current one
			if (graphCopy->GetNodeConnections(currentNode->GetIndex()).size() == 0)
			{
				path.push_back(m_pGraph->GetNode(currentNode->GetIndex()));
				currentNode = stack.back();
				stack.pop_back();
			}
			//add the vertex to the stack, take any of its neighbors, 
			//remove the edge between selected neighbor and that vertex, and set that neighbor as the current vertex.
			else if (currentNode != nullptr && graphCopy->GetNodeConnections(currentNode->GetIndex()).size() != 0)
			{
				stack.push_back(currentNode);
				T_ConnectionType* pConnection = graphCopy->GetNodeConnections(currentNode->GetIndex()).front();
				// getting the other node 
				int otherIndex = pConnection->GetFrom();
				if (otherIndex == currentNode->GetIndex())
					otherIndex = pConnection->GetTo();
				// remove connection
				graphCopy->RemoveConnection(pConnection);
				// set neighbor as current node 
				currentNode = graphCopy->GetNode(otherIndex);
			}
		}
		stack.push_back(currentNode);
		std::reverse(path.begin(), path.end());
		return path;
	}

	template<class T_NodeType, class T_ConnectionType>
	inline void EulerianPath<T_NodeType, T_ConnectionType>::VisitAllNodesDFS(int startIdx, vector<bool>& visited) const
	{
		// mark the visited node
		visited[startIdx] = true;
		// recursively visit any valid connected nodes that were not visited before
		for (T_ConnectionType* connection : m_pGraph->GetNodeConnections(startIdx))
			if (m_pGraph->IsNodeValid(connection->GetTo()) && !visited[connection->GetTo()])
				VisitAllNodesDFS(connection->GetTo(), visited);
	}

	template<class T_NodeType, class T_ConnectionType>
	inline bool EulerianPath<T_NodeType, T_ConnectionType>::IsConnected() const
	{
		const int nrOfNodes = m_pGraph->GetNrOfNodes();
		std::vector<bool> visited(nrOfNodes, false);
		// find a valid starting node that has connections
		int connectedIndex = invalid_node_index; 
		for (int i = 0; i < nrOfNodes; i++)
		{
			if (m_pGraph->IsNodeValid(i) && m_pGraph->GetNodeConnections(i).size() > 0)
			{
				connectedIndex = i; 
				break;
			}
		}
		// if no valid node could be found, return false
		if (connectedIndex == invalid_node_index)
			return false; 

		// start a depth-first-search traversal from a node that has connections
		VisitAllNodesDFS(connectedIndex, visited);

		// if a node was never visited, this graph is not connected
		for (int i = 0; i < nrOfNodes; i++)
			if (m_pGraph->IsNodeValid(i) && !visited[i])
				return false;
		return true;
	}

}