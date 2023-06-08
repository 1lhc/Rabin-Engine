#include <pch.h>
#include "Projects/ProjectTwo.h"
#include "P2_Pathfinding.h"

constexpr float sqrttwo = 1.414214f;

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
			MaxMap[x][y] = node;
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

		push_node(&MaxMap[start.row][start.col], request.settings.debugColoring);
	}
	while (!OpenList.empty()) {
		//	parentNode = Pop cheapest node off Open List.
		Node* parent = popnode();

		//	If parentNode is the Goal Node, then path found(return PathResult::COMPLETE).	
		if (parent->gridPos == goal) {
			std::vector<GridPos> rubberbandVector;
			std::vector<Vec3> smoothVector;
			while (parent != nullptr) {
				if (request.settings.rubberBanding || request.settings.smoothing) {
					rubberbandVector.push_back(parent->gridPos);
					smoothVector.push_back(terrain->get_world_position(parent->gridPos));
				}
				else
					request.path.push_front(terrain->get_world_position(parent->gridPos));
				parent = parent->parent;
			}
			if (request.settings.rubberBanding) {
				request.path.push_front(terrain->get_world_position(rubberbandVector.front()));
				for (int i = 1; i < rubberbandVector.size() - 1;) {
					if (!AStarPather::isSafeToRubberband(rubberbandVector[i + 1], rubberbandVector[i - 1])) {
						// It's safe to rubberband, skip the nextNode
						request.path.push_front(terrain->get_world_position(rubberbandVector[i]));
						++i;
					}
					else
					{
						rubberbandVector.erase(rubberbandVector.begin() + i);
					}
				}
				request.path.push_front(terrain->get_world_position(rubberbandVector.back()));
			}
			if (request.settings.smoothing) {
				if (request.settings.rubberBanding) {
					for (int i = 0; i < smoothVector.size() - 1;) {
						if (sqrt((smoothVector[i].x - smoothVector[i + 1].x) * (smoothVector[i].x - smoothVector[i + 1].x) +
							(smoothVector[i].y - smoothVector[i + 1].y) * (smoothVector[i].y - smoothVector[i + 1].y)) >
							((terrain->get_world_position(MaxMap[0][3].gridPos).z - terrain->get_world_position(MaxMap[0][0].gridPos).z) / 2.f))
						{
							Vec3 midd = (smoothVector[i] + smoothVector[i + 1]) / 2.f;
							smoothVector.insert(smoothVector.begin() + i + 1, midd);
						}
						else
							++i;
					}
				}
				request.path.clear();

				for (int i = 0; i < smoothVector.size() - 1; ++i) {
					int v1 = i - 1, v2 = i, v3 = i + 1, v4 = i + 2;
					if (v1 < 0) {
						v1 = 0;
					}
					if (v4 > static_cast<int>(smoothVector.size() - 1)) {
						v4 = static_cast<int>(smoothVector.size() - 1);
					}
					request.path.push_front(smoothVector[i]);
					for (float t = 0.25f; t <= 0.75f; t += 0.25f) {
						request.path.push_front(DirectX::SimpleMath::Vector3::CatmullRom(smoothVector[v1],
							smoothVector[v2], smoothVector[v3],
							smoothVector[v4], t));
					}

				}
				request.path.push_front(smoothVector.back());
			}
			return PathResult::COMPLETE;
		}
		//	Place parentNode on the Closed List.
		parent->nodeState = onList::Closed;
		if (request.settings.debugColoring) {
			terrain->set_color(parent->gridPos, Colors::Yellow);
		}

		std::vector <Node*> validNeighbors = getNeighbors(*parent);
		for (Node* x : validNeighbors) { //	For(all valid neighboring child nodes of parentNode) {
			// Compute its cost, f(x) = g(x) + h(x)
			float gx = parent->givenCost + BlockDistance(x->gridPos, parent->gridPos);
			float fx = gx + CalculateHeuristic(request.settings.heuristic, x->gridPos, goal) * request.settings.weight;

			//	If child node isn’t on Open or Closed list, put it on Open List.
			if (x->nodeState == onList::Not) {
				x->parent = parent;
				x->finalCost = fx;
				x->givenCost = gx;
				push_node(x, request.settings.debugColoring);
			}
			else if (fx < x->finalCost) { 			//	Else if child node is on Open or Closed List, AND this new one is cheaper,
				x->parent = parent;
				x->givenCost = gx;
				x->finalCost = fx; 					//	then take the old expensive one off both lists and put this new
				if (x->nodeState == onList::Closed) {	//	cheaper one on the Open List.
					x->nodeState = onList::Open;
					push_node(x, request.settings.debugColoring);
				}
			}
		}
		//	If taken too much time this frame(or if request.settings.singleStep == true),
		if (request.settings.singleStep == true) {
			return PathResult::PROCESSING; // abort search for nowand resume next frame(return PathResult::PROCESSING).
		}
	}
	//Open List empty, thus no path possible(return PathResult::IMPOSSIBLE).
	return PathResult::IMPOSSIBLE;
}

void AStarPather::clear_all_nodes()
{
	memcpy(MaxMap, OriginalMap, sizeof(OriginalMap));
}

void AStarPather::push_node(Node* add, bool db)
{
	if (db) {
		terrain->set_color(add->gridPos, Colors::Blue);
	}
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
	OpenList.erase(OpenList.begin() + cheapestIndex);

	return ToReturn;
}

std::vector<AStarPather::Node*> AStarPather::getNeighbors(AStarPather::Node& parentNode) {
	std::vector<AStarPather::Node*> neighbors;

	if (terrain->is_valid_grid_position(parentNode.gridPos.row, parentNode.gridPos.col - 1) &&
		!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col - 1)) { // Mid Left Square

		neighbors.push_back(&MaxMap[parentNode.gridPos.row][parentNode.gridPos.col - 1]);
	}

	if (terrain->is_valid_grid_position(parentNode.gridPos.row, parentNode.gridPos.col + 1) &&
		!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col + 1)) { // Mid Right Square

		neighbors.push_back(&MaxMap[parentNode.gridPos.row][parentNode.gridPos.col + 1]);
	}

	if (terrain->is_valid_grid_position(parentNode.gridPos.row + 1, parentNode.gridPos.col) &&
		!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col)) { // Mid Top Square

		neighbors.push_back(&MaxMap[parentNode.gridPos.row + 1][parentNode.gridPos.col]);
	}

	if (terrain->is_valid_grid_position(parentNode.gridPos.row - 1, parentNode.gridPos.col) &&
		!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col)) { // Mid Bottom Square

		neighbors.push_back(&MaxMap[parentNode.gridPos.row - 1][parentNode.gridPos.col]);
	}

	// Diagonals (First check diagonal itself, then 2 beside)
	if (terrain->is_valid_grid_position(parentNode.gridPos.row + 1, parentNode.gridPos.col - 1) &&
		!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col - 1)) { // Top Left Square

		if (terrain->is_valid_grid_position(parentNode.gridPos.row + 1, parentNode.gridPos.col) &&
			terrain->is_valid_grid_position(parentNode.gridPos.row, parentNode.gridPos.col - 1) &&
			!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col) && // Top Mid Square
			!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col - 1)) {

			neighbors.push_back(&MaxMap[parentNode.gridPos.row + 1][parentNode.gridPos.col - 1]);
		}
	}

	if (terrain->is_valid_grid_position(parentNode.gridPos.row + 1, parentNode.gridPos.col + 1) &&
		!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col + 1)) { // Top Right Square

		if (terrain->is_valid_grid_position(parentNode.gridPos.row + 1, parentNode.gridPos.col) &&
			terrain->is_valid_grid_position(parentNode.gridPos.row, parentNode.gridPos.col + 1) &&
			!terrain->is_wall(parentNode.gridPos.row + 1, parentNode.gridPos.col) && // Top Mid Square
			!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col + 1)) {

			neighbors.push_back(&MaxMap[parentNode.gridPos.row + 1][parentNode.gridPos.col + 1]);
		}
	}

	if (terrain->is_valid_grid_position(parentNode.gridPos.row - 1, parentNode.gridPos.col + 1) &&
		!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col + 1)) { // Bottom Right Square

		if (terrain->is_valid_grid_position(parentNode.gridPos.row - 1, parentNode.gridPos.col) &&
			terrain->is_valid_grid_position(parentNode.gridPos.row, parentNode.gridPos.col + 1) &&
			!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col) && // Bottom Mid Square
			!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col + 1)) {

			neighbors.push_back(&MaxMap[parentNode.gridPos.row - 1][parentNode.gridPos.col + 1]);
		}
	}

	if (terrain->is_valid_grid_position(parentNode.gridPos.row - 1, parentNode.gridPos.col - 1) &&
		!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col - 1)) { // Bottom Left Square

		if (terrain->is_valid_grid_position(parentNode.gridPos.row - 1, parentNode.gridPos.col) &&
			terrain->is_valid_grid_position(parentNode.gridPos.row, parentNode.gridPos.col - 1) &&
			!terrain->is_wall(parentNode.gridPos.row - 1, parentNode.gridPos.col) && // Bottom Mid Square
			!terrain->is_wall(parentNode.gridPos.row, parentNode.gridPos.col - 1)) {

			neighbors.push_back(&MaxMap[parentNode.gridPos.row - 1][parentNode.gridPos.col - 1]);
		}
	}

	return neighbors;
}

float AStarPather::ApplyManhanttan(GridPos start, GridPos goal)
{
	float x = static_cast<float> (fabs(goal.row - start.row) + fabs(goal.col - start.col));
	return x;
}

float AStarPather::ApplyChebyshev(GridPos start, GridPos goal)
{
	return static_cast<float>(std::max(fabs(goal.row - start.row), fabs(goal.col - start.col)));
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
		return ApplyManhanttan(childNode, goal);
		break;
	case Heuristic::CHEBYSHEV:
		return ApplyChebyshev(childNode, goal);
		break;
	case Heuristic::EUCLIDEAN:
		return static_cast<float>(fabs(sqrt((childNode.col - goal.col) * (childNode.col - goal.col) + (childNode.row - goal.row) * (childNode.row - goal.row))));
		break;
	case Heuristic::INCONSISTENT:
		if ((childNode.row + childNode.col) % 2 > 0) {
			return static_cast<float>(fabs(sqrt((childNode.col - goal.col) * (childNode.col - goal.col) + (childNode.row - goal.row) * (childNode.row - goal.row))));
		}
		return 0;
		break;
	case Heuristic::OCTILE:
		return static_cast<float>(std::min(fabs(childNode.col - goal.col), fabs(childNode.row - goal.row)) *
			sqrttwo + std::max(fabs(childNode.col - goal.col), fabs(childNode.row - goal.row)) -
			std::min(fabs(childNode.col - goal.col), fabs(childNode.row - goal.row)));
		break;
	default:
		return static_cast<float>(std::min(fabs(childNode.col - goal.col), fabs(childNode.row - goal.row) *
			sqrttwo + std::max(fabs(childNode.col - goal.col), fabs(childNode.row - goal.row)) -
			std::min(fabs(childNode.col - goal.col), fabs(childNode.row - goal.row))));
		break;
	}
}

bool AStarPather::isSafeToRubberband(GridPos next, GridPos previous)
{
	int startX = std::min(next.row, previous.row);
	int startY = std::min(next.col, previous.col);
	int endX = std::max(next.row, previous.row);
	int endY = std::max(next.col, previous.col);

	for (int x = startX; x <= endX; x++) {
		for (int y = startY; y <= endY; y++) {
			if (terrain->is_wall(x, y))
			{
				return false;
			}
		}
	}

	return true;
}
