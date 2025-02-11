#pragma once
#include "Misc/PathfindingDetails.hpp"

class AStarPather
{
public:
	/*
		The class should be default constructible, so you may need to define a constructor.
		If needed, you can modify the framework where the class is constructed in the
		initialize functions of ProjectTwo and ProjectThree.
	*/

	/* ************************************************** */
	// DO NOT MODIFY THESE SIGNATURES
	bool initialize();
	void shutdown();
	PathResult compute_path(PathRequest& request);
	/* ************************************************** */

	/*
		You should create whatever functions, variables, or classes you need.
		It doesn't all need to be in this header and cpp, structure it whatever way
		makes sense to you.
	*/
	enum class onList { Open, Closed, Not };
	struct Node {
		Node* parent;			// Parent
		GridPos gridPos;		// Node�s location
		float finalCost;		// Final cost f(x)
		float givenCost;		// Given cost g(x)
		onList nodeState;		// On open/closed list?
		Node() :parent{ nullptr }, gridPos{ 0,0 }, finalCost{ 0.f }, givenCost{ 0.f }, nodeState{ onList::Not }{}
	};

	Node MaxMap[40][40];
	Node OriginalMap[40][40];

	void clear_all_nodes();

	std::vector<Node*> OpenList;

	void push_node(Node* add, bool dbcol);
	Node* popnode();

	std::vector<Node*> getNeighbors(Node& parentNode);
	float ApplyManhanttan(GridPos start, GridPos goal);
	float ApplyChebyshev(GridPos start, GridPos goal);

	float BlockDistance(GridPos childNode, GridPos parent);

	float CalculateHeuristic(Heuristic hType, GridPos childNode, GridPos goal);
	bool isSafeToRubberband(GridPos next, GridPos previous);
};