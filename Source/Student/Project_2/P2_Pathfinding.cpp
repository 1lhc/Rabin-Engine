#include <pch.h>
#include "Projects/ProjectTwo.h"
#include "P2_Pathfinding.h"

#pragma region Extra Credit
bool ProjectTwo::implemented_floyd_warshall()
{
	return false;
}

bool ProjectTwo::implemented_goal_bounding()
{
	return false;
}

bool ProjectTwo::implemented_jps_plus()
{
	return false;
}
#pragma endregion

bool AStarPather::initialize()
{
	//for (int i = 0; i < 40; ++i) {
	//	MaxMap[i].gridPos.row = i;
	//}


	for (int x = 0; x < 40; ++x) {
		for (int y = 0; y < 40; ++y) {
			Node node;
			node.gridPos.row = x;
			node.gridPos.col = y;
			node.nodeState = onList::Not;
			node.givenCost = 0.f;
			node.finalCost = 0.f;
			node.parent = NULL;
			MaxMap[x][y] = node;
			OriginalMap[x][y] = node;
			/*MaxMap.push_back(node);
			OriginalMap.push_back(node);*/
		}
	}
	// handle any one-time setup requirements you have

	/*
		If you want to do any map-preprocessing, you'll need to listen
		for the map change message.  It'll look something like this:

		Callback cb = std::bind(&AStarPather::your_function_name, this);
		Messenger::listen_for_message(Messages::MAP_CHANGE, cb);

		There are other alternatives to using std::bind, so feel free to mix it up.
		Callback is just a typedef for std::function<void(void)>, so any std::invoke'able
		object that std::function can wrap will suffice.
	*/

	return true; // return false if any errors actually occur, to stop engine initialization
}

void AStarPather::shutdown()
{
	/*
		Free any dynamically allocated memory or any other general house-
		keeping you need to do during shutdown.
	*/
}

PathResult AStarPather::compute_path(PathRequest& request)
{
	/*
		This is where you handle pathing requests, each request has several fields:

		start/goal - start and goal world positions
		path - where you will build the path upon completion, path should be
			start to goal, not goal to start
		heuristic - which heuristic calculation to use
		weight - the heuristic weight to be applied
		newRequest - whether this is the first request for this path, should generally
			be true, unless single step is on

		smoothing - whether to apply smoothing to the path
		rubberBanding - whether to apply rubber banding
		singleStep - whether to perform only a single A* step
		debugColoring - whether to color the grid based on the A* state:
			closed list nodes - yellow
			open list nodes - blue

			use terrain->set_color(row, col, Colors::YourColor);
			also it can be helpful to temporarily use other colors for specific states
			when you are testing your algorithms

		method - which algorithm to use: A*, Floyd-Warshall, JPS+, or goal bounding,
			will be A* generally, unless you implement extra credit features

		The return values are:
			PROCESSING - a path hasn't been found yet, should only be returned in
				single step mode until a path is found
			COMPLETE - a path to the goal was found and has been built in request.path
			IMPOSSIBLE - a path from start to goal does not exist, do not add start position to path
	*/

	// WRITE YOUR CODE HERE
	GridPos start = terrain->get_grid_position(request.start);
	GridPos goal = terrain->get_grid_position(request.goal);
	if (request.newRequest) {
		//	Initialize everything.Clear Open / Closed Lists.
		clear_all_nodes();
		OpenList.clear();
		//std::fill(std::begin(OpenList), std::end(OpenList), Node());
		//		Push Start Node onto the Open List with cost of f(x) = g(x) + h(x).
		//GridPos start = terrain->get_grid_position(request.start);
		//GridPos goal = terrain->get_grid_position(request.goal);
		//if (request.settings.heuristic == Heuristic::MANHATTAN) {
		//	(find_node(start)).finalCost = goal.row - start.row + goal.col - start.col;
		//}
		Node start_node;
		switch (request.settings.heuristic) {
		case Heuristic::MANHATTAN:
			/*for (Node x : MaxMap) {*/
			for (auto& rows : MaxMap) // Iterating over rows
			{
				for (auto& elem : rows)
				{
					if (elem.gridPos == start) {
						start_node = elem;
					}
				}
			}
			//}
			start_node.finalCost = static_cast<float>(goal.row - start.row + goal.col - start.col);
			break;
		default:
			break;
		}
		push_node(&start_node);
	}
	while (!OpenList.empty()) {
		//	parentNode = Pop cheapest node off Open List.
		Node* parent = popnode();
		//		If parentNode is the Goal Node, then path found(return PathResult::COMPLETE).
		Node goal_node;
		/*for (Node& x : MaxMap) {
			if (x.gridPos == goal) {
				goal_node = x;
			}
		}*/

		for (auto& rows : MaxMap) // Iterating over rows
		{
			for (auto& elem : rows)
			{
				if (elem.gridPos == goal) {
					goal_node = elem;
				}
			}
		}
		if (parent == &goal_node) {
			return PathResult::COMPLETE;
		}
		//		Place parentNode on the Closed List.
		parent->nodeState = onList::Closed;
		//		For(all valid neighboring child nodes of parentNode) {

		//		Compute its cost, f(x) = g(x) + h(x)
		//			If child node isn’t on Open or Closed list, put it on Open List.
		//			Else if child node is on Open or Closed List, AND this new one is cheaper,
		//			then take the old expensive one off both listsand put this new
		//			cheaper one on the Open List.
	}
	//	If taken too much time this frame(or if request.settings.singleStep == true),
	//		abort search for nowand resume next frame(return PathResult::PROCESSING).
	//}
	//Open List empty, thus no path possible(return PathResult::IMPOSSIBLE).


	// Just sample code, safe to delete
	//GridPos start = terrain->get_grid_position(request.start);
	//GridPos goal = terrain->get_grid_position(request.goal);
	terrain->set_color(start, Colors::Orange);
	terrain->set_color(goal, Colors::Orange);
	request.path.push_back(request.start);
	request.path.push_back(request.goal);
	return PathResult::COMPLETE;
}

void AStarPather::clear_all_nodes()
{
	//MaxMap = OriginalMap;
	memcpy(MaxMap, OriginalMap, sizeof(OriginalMap));
}

void AStarPather::push_node(Node* add)
{
	add->nodeState = onList::Open;
	OpenList.push_back(add);
}

//void AStarPather::pop_node()
//{
//	int cheapestIndex = 0; 
//	float cheapestValue = OpenList[0]->finalCost;  // Assume the first node is the cheapest
//
//	for (int i = 1; i < OpenList.size(); ++i) {
//		if (OpenList[i]->finalCost < cheapestValue) {
//			cheapestIndex = i;
//			cheapestValue = OpenList[i]->finalCost;
//		}
//	}
//	OpenList[cheapestIndex]->nodeState = onList::Closed;
//	OpenList[cheapestIndex] = OpenList.back();
//	OpenList.pop_back();
//}

AStarPather::Node* AStarPather::popnode()
{
	int cheapestIndex = 0;
	float cheapestValue = OpenList[0]->finalCost;  // Assume the first node is the cheapest

	for (int i = 1; i < OpenList.size(); ++i) {
		if (OpenList[i]->finalCost < cheapestValue) {
			cheapestIndex = i;
			cheapestValue = OpenList[i]->finalCost;
		}
	}
	OpenList[cheapestIndex]->nodeState = onList::Closed;
	Node* x = OpenList[cheapestIndex];
	OpenList[cheapestIndex] = OpenList.back();
	OpenList.pop_back();
	return x;
}

void AStarPather::update_node() {}



std::vector<AStarPather::Node*> AStarPather::getNeighbors(AStarPather::Node& parentNode) {
	std::vector<AStarPather::Node*> neighbors;

	// Logic to find and add valid neighboring nodes to the 'neighbors' vector
	// This can include checking adjacent cells, edges, or any other relevant criteria
	if (!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col - 1)) { // Mid Left Square
		neighbors.push_back(&MaxMap[parentNode.gridPos.row][parentNode.gridPos.col - 1]);
	}

	if (!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col + 1)) { // Mid Right Square
		neighbors.push_back(&MaxMap[parentNode.gridPos.row][parentNode.gridPos.col + 1]);
	}

	if (!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col)) { // Mid Top Square
		neighbors.push_back(&MaxMap[parentNode.gridPos.row + 1][parentNode.gridPos.col - 1]);
	}

	if (!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col)) { // Mid Bottom Square
		neighbors.push_back(&MaxMap[parentNode.gridPos.row - 1][parentNode.gridPos.col - 1]);
	}

	// Diagonals (First check diagonal itself, then 2 beside)
	if (!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col - 1)) { // Top Left Square
		if (!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col) && // Top Mid Square
			!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col - 1)) {
			neighbors.push_back(&MaxMap[parentNode.gridPos.row + 1][parentNode.gridPos.col - 1]);
		}
	}

	if (!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col + 1)) { // Top Right Square
		if (!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col) && // Top Mid Square
			!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col + 1)) {
			neighbors.push_back(&MaxMap[parentNode.gridPos.row + 1][parentNode.gridPos.col + 1]);
		}
	}

	if (!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col + 1)) { // Bottom Right Square
		if (!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col) && // Bottom Mid Square
			!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col + 1)) {
			neighbors.push_back(&MaxMap[parentNode.gridPos.row - 1][parentNode.gridPos.col + 1]);
		}
	}

	if (!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col - 1)) { // Bottom Left Square
		if (!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col) && // Bottom Mid Square
			!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col - 1)) {
			neighbors.push_back(&MaxMap[parentNode.gridPos.row - 1][parentNode.gridPos.col - 1]);
		}
	}

	return neighbors;
}


//AStarPather::Node* AStarPather::find_node(GridPos pos)
//{
//	for (Node x : MaxMap) {
//		if (x.gridPos == pos) {
//			return x;
//		}
//	}
//	return nullptr;
//}
