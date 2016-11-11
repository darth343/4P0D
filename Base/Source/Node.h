#ifndef NODE_H
#define NODE_H
#include "Tile.h"
struct Node
{
	Node() : parent(nullptr) {};
	Node(Tile tile, Node* parent = nullptr) : parent(parent)
	{
		posX = tile.Pos.x;
		posY = tile.Pos.y;
	};
	~Node(){};
	int posX;
	int posY;

	int nodeID;

	Node * parent;

	float G;
	float H;
	float getF(){ return G + H; };
	float Distance(Node* End)
	{
		float x = abs(posX - End->posX);
		float y = abs(posY - End->posY);
		return (x + y) * 10;
	}
};
#endif