#ifndef PATHFINDER_H
#define PATHFINDER_H
#include <vector>
#include "Map.h"
#include "Tile.h"
#include "Node.h"
using std::vector;
class Pathfinder
{
public:
	Pathfinder();
	~Pathfinder();

	Node * getNextNodeFromOpenList();
	void FindPath(Tile currentTile, Tile endTile, CMap* m_cMap);
	void ContinueSearch(CMap* m_cMap);
	void openNode(int posX, int posY, float newCost, Node * parent, CMap* m_cMap);
	void ClearLists();
	vector<Node *> openList;
	vector<Node *> visitedList;
	vector<Tile> pathToEnd;
	Node* getStart();
	bool initializedStartandEnd;
	bool found;
private:
	Node* start;
	Node* end;

};

#endif // !PATHFINDER_H
