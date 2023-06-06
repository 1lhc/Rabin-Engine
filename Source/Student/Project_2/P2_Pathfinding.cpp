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
			node.parent = nullptr;
			OriginalMap[x][y] = node;
			//node.validNeighbors = getNeighbors(node);
			MaxMap[x][y] = node;

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
		//	Initialize everything. Clear Open / Closed Lists.
		clear_all_nodes();
		OpenList.clear();
		//	Push Start Node onto the Open List with cost of f(x) = g(x) + h(x).
		MaxMap[start.row][start.col].parent = nullptr;
		MaxMap[start.row][start.col].givenCost = 0.f;
		MaxMap[start.row][start.col].finalCost = CalculateHeuristic(request.settings.heuristic, MaxMap[start.row][start.col].gridPos, goal) * request.settings.weight;

		push_node(&MaxMap[start.row][start.col]);
	}
	while (!OpenList.empty()) {
		//	parentNode = Pop cheapest node off Open List.
		Node* parent = popnode();

		//	If parentNode is the Goal Node, then path found(return PathResult::COMPLETE).	
		if (parent->gridPos == goal) {
			while (parent != nullptr) {
				std::cout << parent->gridPos.row << " col:" << parent->gridPos.col << "\n";
				request.path.push_front(terrain->get_world_position(parent->gridPos));
				parent = parent->parent;
				std::cout << "TRACING ";
			}
			std::cout << "COMPLETE ";
			return PathResult::COMPLETE;
		}
		//	Place parentNode on the Closed List.
		parent->nodeState = onList::Closed;
		//terrain->set_color(parent->gridPos, Colors::Yellow);
		//parent = OpenList.back();
		//OpenList.pop_back();
		//popnode();
		//terrain->set_color(parent->gridPos, Colors::Yellow);

		std::vector <Node*> validNeighbors = getNeighbors(*parent);
		for (Node* x : validNeighbors) { //	For(all valid neighboring child nodes of parentNode) {
			// Compute its cost, f(x) = g(x) + h(x)
			// 1. determine vertical/horizontal/diagonal
			// float givencost = parent->givenCost; // 2. vert/hor = 1; dia = sqrt 2
			// float cost = parent->finalCost; // 3.
			float gx = parent->givenCost + BlockDistance(x->gridPos, parent->gridPos);
			float fx = gx + CalculateHeuristic(request.settings.heuristic, x->gridPos, goal) * request.settings.weight;

			//	If child node isn’t on Open or Closed list, put it on Open List.
			if (x->nodeState == onList::Not) {
				/*x->nodeState = onList::Open;*/
				x->parent = parent;
				x->finalCost = fx;
				x->givenCost = gx;
				push_node(x);
				//terrain->set_color(x->gridPos, Colors::Blue);
			}
			else if (fx < x->finalCost) { 			//	Else if child node is on Open or Closed List, AND this new one is cheaper,
				x->parent = parent;
				x->givenCost = gx;
				x->finalCost = fx; 					//	then take the old expensive one off both lists and put this new
				if (x->nodeState == onList::Closed) {	//	cheaper one on the Open List.
					x->nodeState = onList::Open;
					push_node(x);
				}
			}
		}
		//	If taken too much time this frame(or if request.settings.singleStep == true),
		if (request.settings.singleStep == true) {
			return PathResult::PROCESSING; // abort search for nowand resume next frame(return PathResult::PROCESSING).
		}
	}
	//Open List empty, thus no path possible(return PathResult::IMPOSSIBLE).
	std::cout << "IMPOSSIBLE ";
	return PathResult::IMPOSSIBLE;


	// Just sample code, safe to delete
	//GridPos start = terrain->get_grid_position(request.start);
	//GridPos goal = terrain->get_grid_position(request.goal);
	/*terrain->set_color(start, Colors::Orange);
	terrain->set_color(goal, Colors::Orange);*/
	/*request.path.push_back(request.start);*/
	/*request.path.push_back(request.goal);*/
	//return PathResult::COMPLETE;
}

void AStarPather::clear_all_nodes()
{
	//MaxMap = OriginalMap;
	memcpy(MaxMap, OriginalMap, sizeof(OriginalMap));
}

void AStarPather::push_node(Node* add)
{
	terrain->set_color(add->gridPos, Colors::Blue);
	add->nodeState = onList::Open;
	OpenList.push_back(add);
}

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

	Node* ToReturn = &MaxMap[OpenList[cheapestIndex]->gridPos.row][OpenList[cheapestIndex]->gridPos.col];
	MaxMap[OpenList[cheapestIndex]->gridPos.row][OpenList[cheapestIndex]->gridPos.col].nodeState = onList::Closed;
	//OpenList[cheapestIndex]->nodeState = onList::Closed;
	OpenList.erase(OpenList.begin() + cheapestIndex);
	//terrain->set_color(OpenList[cheapestIndex]->gridPos, Colors::Yellow);
	terrain->set_color(ToReturn->gridPos, Colors::Yellow);

	/*Node* x = OpenList[cheapestIndex];
	OpenList[cheapestIndex] = OpenList.back();
	OpenList.pop_back();*/
	return ToReturn;
	//return OpenList[cheapestIndex];
}

std::vector<AStarPather::Node*> AStarPather::getNeighbors(AStarPather::Node& parentNode) {
	std::vector<AStarPather::Node*> neighbors;

	if (terrain->is_valid_grid_position(parentNode.gridPos.row, parentNode.gridPos.col - 1) &&
		!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col - 1)) { // Mid Left Square
		//MaxMap[parentNode.gridPos.row][parentNode.gridPos.col - 1].parent = &parentNode;
		neighbors.push_back(&MaxMap[parentNode.gridPos.row][parentNode.gridPos.col - 1]);
	}

	if (terrain->is_valid_grid_position(parentNode.gridPos.row, parentNode.gridPos.col + 1) &&
		!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col + 1)) { // Mid Right Square
		/*MaxMap[parentNode.gridPos.row][parentNode.gridPos.col + 1].parent = &parentNode;*/
		neighbors.push_back(&MaxMap[parentNode.gridPos.row][parentNode.gridPos.col + 1]);
	}

	if (terrain->is_valid_grid_position(parentNode.gridPos.row + 1, parentNode.gridPos.col) &&
		!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col)) { // Mid Top Square
		//MaxMap[parentNode.gridPos.row + 1][parentNode.gridPos.col].parent = &parentNode;
		neighbors.push_back(&MaxMap[parentNode.gridPos.row + 1][parentNode.gridPos.col]);
	}

	if (terrain->is_valid_grid_position(parentNode.gridPos.row - 1, parentNode.gridPos.col) &&
		!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col)) { // Mid Bottom Square
		//MaxMap[parentNode.gridPos.row - 1][parentNode.gridPos.col].parent = &parentNode;
		//MaxMap[parentNode.gridPos.row - 1][parentNode.gridPos.col].givenCost += 1.f;
		neighbors.push_back(&MaxMap[parentNode.gridPos.row - 1][parentNode.gridPos.col]);
	}

	// Diagonals (First check diagonal itself, then 2 beside)
	if (terrain->is_valid_grid_position(parentNode.gridPos.row + 1, parentNode.gridPos.col - 1) &&
		!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col - 1)) { // Top Left Square

		if (terrain->is_valid_grid_position(parentNode.gridPos.row + 1, parentNode.gridPos.col) &&
			terrain->is_valid_grid_position(parentNode.gridPos.row, parentNode.gridPos.col - 1) &&
			!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col) && // Top Mid Square
			!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col - 1)) {

			//MaxMap[parentNode.gridPos.row + 1][parentNode.gridPos.col - 1].parent = &parentNode;
			neighbors.push_back(&MaxMap[parentNode.gridPos.row + 1][parentNode.gridPos.col - 1]);
		}
	}

	if (terrain->is_valid_grid_position(parentNode.gridPos.row + 1, parentNode.gridPos.col + 1) &&
		!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col + 1)) { // Top Right Square

		if (terrain->is_valid_grid_position(parentNode.gridPos.row + 1, parentNode.gridPos.col) &&
			terrain->is_valid_grid_position(parentNode.gridPos.row, parentNode.gridPos.col + 1) &&
			!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col) && // Top Mid Square
			!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col + 1)) {

			//MaxMap[parentNode.gridPos.row + 1][parentNode.gridPos.col + 1].parent = &parentNode;
			neighbors.push_back(&MaxMap[parentNode.gridPos.row + 1][parentNode.gridPos.col + 1]);
		}
	}

	if (terrain->is_valid_grid_position(parentNode.gridPos.row - 1, parentNode.gridPos.col + 1) &&
		!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col + 1)) { // Bottom Right Square

		if (terrain->is_valid_grid_position(parentNode.gridPos.row - 1, parentNode.gridPos.col) &&
			terrain->is_valid_grid_position(parentNode.gridPos.row, parentNode.gridPos.col + 1) &&
			!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col) && // Bottom Mid Square
			!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col + 1)) {

			//MaxMap[parentNode.gridPos.row - 1][parentNode.gridPos.col + 1].parent = &parentNode;
			neighbors.push_back(&MaxMap[parentNode.gridPos.row - 1][parentNode.gridPos.col + 1]);
		}
	}

	if (terrain->is_valid_grid_position(parentNode.gridPos.row - 1, parentNode.gridPos.col - 1) &&
		!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col - 1)) { // Bottom Left Square

		if (terrain->is_valid_grid_position(parentNode.gridPos.row - 1, parentNode.gridPos.col) &&
			terrain->is_valid_grid_position(parentNode.gridPos.row, parentNode.gridPos.col - 1) &&
			!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col) && // Bottom Mid Square
			!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col - 1)) {

			//MaxMap[parentNode.gridPos.row - 1][parentNode.gridPos.col - 1].parent = &parentNode;
			neighbors.push_back(&MaxMap[parentNode.gridPos.row - 1][parentNode.gridPos.col - 1]);
		}
	}

	return neighbors;
}

float AStarPather::ApplyManhanttan(GridPos start, GridPos goal)
{
	float x = static_cast<float> (abs(goal.row - start.row + goal.col - start.col));
	return x;
}

float AStarPather::ApplyChebyshev(GridPos start, GridPos goal)
{
	return static_cast<float>(std::max(abs(goal.row - start.row), abs(goal.col - start.col)));
}

float AStarPather::BlockDistance(GridPos childNode, GridPos parent)
{
	return static_cast<float>(sqrt((childNode.row - parent.row) * (childNode.row - parent.row) +
		(childNode.col - parent.col) * (childNode.col - parent.col)));
}

float AStarPather::CalculateHeuristic(Heuristic hType, GridPos childNode, GridPos goal)
{
	switch (hType) {
	case Heuristic::MANHATTAN:
		//cost += static_cast<float>(goal.row - x->gridPos.row + goal.col - x->gridPos.col);
		return ApplyManhanttan(childNode, goal);
		break;
	default:
		break;
	}

	return 1.0f;
}
