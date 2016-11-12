#include "Pathfinder.h"

Pathfinder::Pathfinder()
: initializedStartandEnd(false)
, found(false)
, start(NULL)
, end(NULL)
{
}

Pathfinder::~Pathfinder()
{
	ClearLists();
    if (start)
	delete start;
    if (end)
	delete end;
}

void Pathfinder::ClearLists()
{
	while (openList.size() > 0)
	{
		Node * temp;
		temp = openList.back();
		openList.pop_back();
		delete temp;
		temp = NULL;
	}
	openList.clear();
	while (visitedList.size() > 0)
	{
		Node * temp;
		temp = visitedList.back();
		visitedList.pop_back();
		if (temp)
		delete temp;
	}
	visitedList.clear();
	pathToEnd.clear();
}

void Pathfinder::FindPath(Tile startTile, Tile endTile, CMap* m_cMap)
{
	for (int i = 0; i < 20; ++i)
	{
		if (!initializedStartandEnd)
		{
			// Clear/Reset Lists
			openList.clear();
			visitedList.clear();
			pathToEnd.clear();
			found = false;
			// Initialize Start and End
			start = m_cMap->theNodeMap[(int)startTile.Pos.y][(int)startTile.Pos.x];
			start->G = 0;
			end = m_cMap->theNodeMap[(int)endTile.Pos.y][(int)endTile.Pos.x];
			start->H = start->Distance(end);
			openList.push_back(start);
			initializedStartandEnd = true;
		}
		if (initializedStartandEnd && !found)
		{
			ContinueSearch(m_cMap);
		}
	}
}

Node* Pathfinder::getNextNodeFromOpenList()
{
	float lowestF = 9999;
	int nodeIndex = 0;

	Node* nextNode = nullptr;
	for (int i = 0; i < openList.size(); ++i)
	{
		if (openList[i]->getF() < lowestF)
		{
			lowestF = openList[i]->getF();
			nodeIndex = i;
		}
		else if (openList[i]->getF() == lowestF)
		{
			nodeIndex = i;
		}
	}
	if (abs(openList[0]->posX) > 1000 || abs(openList[0]->posY) > 1000)
	{
		int a = 0;
	}
	if (nodeIndex >= 0)
	{
		nextNode = openList[nodeIndex];
		visitedList.push_back(nextNode);
		openList.erase(openList.begin() + nodeIndex);
	}
	return nextNode;
}

void Pathfinder::ContinueSearch(CMap* m_cMap)
{
	if (openList.empty())
	{
		initializedStartandEnd = false;
		//cout << "BROKE" << endl;
		return;
	}

	Node * currentNode = getNextNodeFromOpenList();

	if (currentNode->nodeID == end->nodeID)
	{
		found = true;
		Node* getPath;
		for (getPath = currentNode; getPath != nullptr; getPath = getPath->parent)
		{
			pathToEnd.push_back(m_cMap->theMap[getPath->posY][getPath->posX]);
		}
		return;
	}
	else
	{
		//Right Side
		openNode(currentNode->posX + 1, currentNode->posY, currentNode->G + 10, currentNode, m_cMap);

		//Left Side
		openNode(currentNode->posX - 1, currentNode->posY, currentNode->G + 10, currentNode, m_cMap);

		//Above
		openNode(currentNode->posX, currentNode->posY + 1, currentNode->G + 10, currentNode, m_cMap);

		//Below
		openNode(currentNode->posX, currentNode->posY - 1, currentNode->G + 10, currentNode, m_cMap);
	}

	for (int i = 0; i < openList.size(); ++i)
	{
		if (currentNode->nodeID == openList[i]->nodeID)
		{
			openList.erase(openList.begin() + i);
		}
	}
}

void Pathfinder::openNode(int posX, int posY, float newCost, Node * parent, CMap* m_cMap)
{
	if (m_cMap->theMap[posY][posX].shouldCollide)
	{
		return;
	}
	int id = m_cMap->theNodeMap[posY][posX]->nodeID;
	for (int i = 0; i < visitedList.size(); ++i)
	{
		if (id == visitedList[i]->nodeID)
		{
			return; // Dont retrace back
		}
	}
	bool InOpenList = false;
	Node newChild = Node(m_cMap->theMap[posY][posX], parent);
	newChild.nodeID = id;
	newChild.G = newCost;
	newChild.H = parent->Distance(end);
	//float ftest = newChild->getF();
	//ftest;
	for (int i = 0; i < openList.size(); ++i)
	{
		if (id == openList[i]->nodeID)
		{
			float newF = newChild.G + newCost + newChild.H;
			if (openList[i]->getF() > newF)
			{
				openList[i]->G = newChild.G + newCost;
				openList[i]->parent = parent;
			}
			else
			{
				return;
			}
		}
	}
	Node * temp = new Node();
	*temp = newChild;
	openList.push_back(temp);
}

Node* Pathfinder::getStart()
{
	return start;
}


























