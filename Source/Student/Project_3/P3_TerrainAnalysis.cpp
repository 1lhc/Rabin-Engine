#include <pch.h>
#include "Terrain/TerrainAnalysis.h"
#include "Terrain/MapMath.h"
#include "Agent/AStarAgent.h"
#include "Terrain/MapLayer.h"
#include "Projects/ProjectThree.h"

#include <iostream>

bool ProjectThree::implemented_fog_of_war() const // extra credit
{
	return false;
}

float distance_to_closest_wall(int row, int col)
{
	/*
		Check the euclidean distance from the given cell to every other wall cell,
		with cells outside the map bounds treated as walls, and return the smallest
		distance.  Make use of the is_valid_grid_position and is_wall member
		functions in the global terrain to determine if a cell is within map bounds
		and a wall, respectively.
	*/

	// WRITE YOUR CODE HERE
   // float minDistance = FLT_MAX; // Initialize minDistance to a large value

   //// Iterate over all cells in the map
   // for (int i = 0; i < terrain->get_map_height(); i++)
   // {
   //     for (int j = 0; j < terrain->get_map_width(); j++)
   //     {
   //         // Check if the cell is a valid grid position and a wall
   //         if ((terrain->is_valid_grid_position(i, j) && terrain->is_wall(i, j)) || !terrain->is_valid_grid_position(i, j))
   //         {
   //             // Calculate the Euclidean distance between the current cell and the input cell (row, col)
   //             float distance = static_cast<float>(sqrt(pow(j - col, 2) + pow(i - row, 2)));

   //             // Update minDistance if the calculated distance is smaller
   //             if (distance < minDistance)
   //             {
   //                 minDistance = distance;
   //             }
   //         }
   //     }
   // }

   // return minDistance;
	float minDistance = FLT_MAX; // Initialize minDistance to a large value

	// Iterate over all cells in the map, including cells outside the map bounds
	for (int i = -1; i <= terrain->get_map_height(); i++)
	{
		for (int j = -1; j <= terrain->get_map_width(); j++)
		{
			// Check if the cell is within the map bounds or outside
			bool isValidPosition = terrain->is_valid_grid_position(i, j);
			bool isWall = !isValidPosition || terrain->is_wall(i, j);

			if (isWall)
			{
				// Calculate the Euclidean distance between the current cell and the input cell (row, col)
				float distance = static_cast<float>(sqrt(pow(j - col, 2) + pow(i - row, 2)));

				// Update minDistance if the calculated distance is smaller
				if (distance < minDistance)
				{
					minDistance = distance;
				}
			}
		}
	}

	return minDistance;
	// return 0.0f; // REPLACE THIS
}

bool is_clear_path(int row0, int col0, int row1, int col1)
{
	/*
		Two cells (row0, col0) and (row1, col1) are visible to each other if a line
		between their centerpoints doesn't intersect the four boundary lines of every
		wall cell.  You should puff out the four boundary lines by a very tiny amount
		so that a diagonal line passing by the corner will intersect it.  Make use of the
		line_intersect helper function
		bool line_intersect(const Vec2 & line0P0, const Vec2 & line0P1, const Vec2 & line1P0, const Vec2 & line1P1)
		for the intersection test and the is_wall member function in the global terrain to
		determine if a cell is a wall or not.
	*/
	// WRITE YOUR CODE HERE
	Vec2 zhongjian0 = { terrain->get_world_position(row0, col0).z, terrain->get_world_position(row0, col0).x };
	Vec2 zhongjian1 = { terrain->get_world_position(row1, col1).z, terrain->get_world_position(row1, col1).x };
	float halfcellLength = terrain->get_world_position(0, 0).z;

	// Puff out the four boundary lines slightly
	float epsilon = 0.001f;

	int minRow = std::min(row0, row1);
	int minCol = std::min(col0, col1);

	int maxRow = std::max(row0, row1);
	int maxCol = std::max(col0, col1);
	// Check if the line between center0 and center1 intersects the four boundary lines of every wall cell
   for (int r = minRow; r <= maxRow; r++) {
   	for (int c = minCol; c <= maxCol; c++) {
	//for (int r = 0; r < terrain->get_map_height(); r++) {
	//	for (int c = 0; c < terrain->get_map_width(); c++) {
			if (terrain->is_wall(r, c)) {
				Vec2 bottomLeft = { terrain->get_world_position(r, c).z - halfcellLength, terrain->get_world_position(r, c).x - halfcellLength }; // Center point of wall cell (r, c)
				Vec2 bottomRight = { bottomLeft.x + 2 * halfcellLength, bottomLeft.y };
				Vec2 topLeft = { bottomLeft.x, bottomLeft.y + 2 * halfcellLength };
				Vec2 topRight = { topLeft.x + 2 * halfcellLength, topLeft.y };
				// Check intersection with the four boundary lines of the wall cell
				if (line_intersect(zhongjian0, zhongjian1, bottomLeft, bottomRight) ||
					line_intersect(zhongjian0, zhongjian1, bottomLeft, topLeft) ||
					line_intersect(zhongjian0, zhongjian1, topLeft, topRight) ||
					line_intersect(zhongjian0, zhongjian1, bottomRight, topRight)) {
					return false; // Line intersects a wall cell's boundary lines, path is not clear
				}
			}
		}
	}

	return true; // No intersection with any wall cell's boundary lines, path is clear
}

void analyze_openness(MapLayer<float>& layer)
{
	/*
		Mark every cell in the given layer with the value 1 / (d * d),
		where d is the distance to the closest wall or edge.  Make use of the
		distance_to_closest_wall helper function.  Walls should not be marked.
	*/

	// WRITE YOUR CODE HERE
	for (int r = 0; r < terrain->get_map_height(); r++) {
		for (int c = 0; c < terrain->get_map_width(); c++) {
			if (!terrain->is_wall(r, c))
				layer.set_value(r, c, (1 / (distance_to_closest_wall(r, c) * distance_to_closest_wall(r, c))));
		}
	}
}

void analyze_visibility(MapLayer<float>& layer)
{
	/*
		Mark every cell in the given layer with the number of cells that
		are visible to it, divided by 160 (a magic number that looks good).  Make sure
		to cap the value at 1.0 as well.

		Two cells are visible to each other if a line between their centerpoints doesn't
		intersect the four boundary lines of every wall cell.  Make use of the is_clear_path
		helper function.
	*/

	// WRITE YOUR CODE HERE
	int numRows = terrain->get_map_height();
	int numCols = terrain->get_map_width();

	// Calculate visibility for each cell
	for (int r = 0; r < numRows; r++) {
		for (int c = 0; c < numCols; c++) {
			float visibility = 0.0f;

			for (int i = 0; i < numRows; i++) {
				for (int j = 0; j < numCols; j++) {
					if (i == r && j == c) {
						++visibility;  // The cell is always visible to itself
					}
					else {
						bool isClearPath = is_clear_path(r, c, i, j);
						visibility += static_cast<float>(isClearPath);
					}
				}
			}

			visibility /= 160.0f;  // Divide by the magic number
			visibility = std::min(1.0f, visibility);  // Cap the value at 1.0
			layer.set_value(r, c, visibility);
		}
	}
}

void analyze_visible_to_cell(MapLayer<float>& layer, int row, int col)
{
	/*
		For every cell in the given layer mark it with 1.0 if it is visible to the given cell,
		0.5 if it isn't visible but is next to a visible cell,
		or 0.0 otherwise.

		Two cells are visible to each other if a line between their centerpoints doesn't
		intersect the four boundary lines of every wall cell.  Make use of the is_clear_path
		helper function.
	*/

	// WRITE YOUR CODE HERE
	int numRows = terrain->get_map_height();
	int numCols = terrain->get_map_width();

	for (int r = 0; r < numRows; r++) {
		for (int c = 0; c < numCols; c++) {
			if (!terrain->is_wall(r, c)) {
				if (r == row && c == col) {
					layer.set_value(r, c, 1.0f); // Given cell is always visible to itself
				}
				else if (is_clear_path(row, col, r, c)) {
					layer.set_value(r, c, 1.0f); // Visible cell

				}
				else if (terrain->is_valid_grid_position(r - 1, c - 1) && layer.get_value(r - 1, c - 1) == 1.0f || //bottom
					terrain->is_valid_grid_position(r - 1, c) && layer.get_value(r - 1, c) == 1.0f ||
					terrain->is_valid_grid_position(r - 1, c + 1) && layer.get_value(r - 1, c + 1) == 1.0f ||

					terrain->is_valid_grid_position(r, c - 1) && layer.get_value(r, c - 1) == 1.0f || // mid
					terrain->is_valid_grid_position(r, c + 1) && layer.get_value(r, c + 1) == 1.0f ||

					terrain->is_valid_grid_position(r + 1, c - 1) && layer.get_value(r + 1, c - 1) == 1.0f || // top
					terrain->is_valid_grid_position(r + 1, c) && layer.get_value(r + 1, c) == 1.0f ||
					terrain->is_valid_grid_position(r + 1, c + 1) && layer.get_value(r + 1, c + 1) == 1.0f) {
				/*	if(terrain->is_wall())*/

					layer.set_value(r, c, 0.5f); // Cell next to a visible cell
				}
				//else if (std::abs(r - row) <= 1 && std::abs(c - col) <= 1) { // Only checks 
				//	layer.set_value(r, c, 0.5f); // Cell next to a visible cell
				//}
				else {
					layer.set_value(r, c, 0.0f); // Not visible cell
				}
			}
		}
	}
	
}

void analyze_agent_vision(MapLayer<float>& layer, const Agent* agent)
{
	/*
		For every cell in the given layer that is visible to the given agent,
		mark it as 1.0, otherwise don't change the cell's current value.

		You must consider the direction the agent is facing.  All of the agent data is
		in three dimensions, but to simplify you should operate in two dimensions, the XZ plane.

		Take the dot product between the view vector and the vector from the agent to the cell,
		both normalized, and compare the cosines directly instead of taking the arccosine to
		avoid introducing floating-point inaccuracy (larger cosine means smaller angle).

		Give the agent a field of view slighter larger than 180 degrees.

		Two cells are visible to each other if a line between their centerpoints doesn't
		intersect the four boundary lines of every wall cell.  Make use of the is_clear_path
		helper function.
	*/

	// WRITE YOUR CODE HERE

	const int layerHeight = terrain->get_map_height();
	const int layerWidth = terrain->get_map_width();

	// Convert agent's position to 2D coordinates
	int agentRow = static_cast<int>(agent->get_position().y);
	int agentCol = static_cast<int>(agent->get_position().x);

	// Calculate agent's view direction vector
	float agentDirectionX = std::cos(agent->get_yaw());
	float agentDirectionZ = std::sin(agent->get_yaw());

	// Calculate field of view boundaries
	const float fieldOfView = 190.0f;  // slightly larger than 180 degrees
	const float halfFieldOfView = fieldOfView / 2.0f;
	float minCosine = static_cast<float>(std::cos(halfFieldOfView * (M_PI / 180.0f)));

	// Iterate over all cells in the layer
	for (int row = 0; row < layerHeight; ++row)
	{
		for (int col = 0; col < layerWidth; ++col)
		{
			// Calculate the vector from agent to cell
			float cellVectorX = col + 0.5f - agent->get_position().x;
			float cellVectorZ = row + 0.5f - agent->get_position().z;

			// Normalize the cell vector
			float cellVectorLength = std::sqrt(cellVectorX * cellVectorX + cellVectorZ * cellVectorZ);
			cellVectorX /= cellVectorLength;
			cellVectorZ /= cellVectorLength;

			// Calculate the dot product between view vector and cell vector
			float dotProduct = agentDirectionX * cellVectorX + agentDirectionZ * cellVectorZ;

			// Check if the cell is within agent's field of view
			if (dotProduct >= minCosine && is_clear_path(agentRow, agentCol, row, col))
			{
				// Mark the cell as visible
				layer.set_value(row, col, 1.0f);
			}
		}
	}
}

void propagate_solo_occupancy(MapLayer<float>& layer, float decay, float growth)
{
	/*
		For every cell in the given layer:

			1) Get the value of each neighbor and apply decay factor
			2) Keep the highest value from step 1
			3) Linearly interpolate from the cell's current value to the value from step 2
			   with the growing factor as a coefficient.  Make use of the lerp helper function.
			4) Store the value from step 3 in a temporary layer.
			   A float[40][40] will suffice, no need to dynamically allocate or make a new MapLayer.

		After every cell has been processed into the temporary layer, write the temporary layer into
		the given layer;
	*/

	// WRITE YOUR CODE HERE
}

void propagate_dual_occupancy(MapLayer<float>& layer, float decay, float growth)
{
	/*
		Similar to the solo version, but the values range from -1.0 to 1.0, instead of 0.0 to 1.0

		For every cell in the given layer:

		1) Get the value of each neighbor and apply decay factor
		2) Keep the highest ABSOLUTE value from step 1
		3) Linearly interpolate from the cell's current value to the value from step 2
		   with the growing factor as a coefficient.  Make use of the lerp helper function.
		4) Store the value from step 3 in a temporary layer.
		   A float[40][40] will suffice, no need to dynamically allocate or make a new MapLayer.

		After every cell has been processed into the temporary layer, write the temporary layer into
		the given layer;
	*/

	// WRITE YOUR CODE HERE
}

void normalize_solo_occupancy(MapLayer<float>& layer)
{
	/*
		Determine the maximum value in the given layer, and then divide the value
		for every cell in the layer by that amount.  This will keep the values in the
		range of [0, 1].  Negative values should be left unmodified.
	*/

	// WRITE YOUR CODE HERE
}

void normalize_dual_occupancy(MapLayer<float>& layer)
{
	/*
		Similar to the solo version, but you need to track greatest positive value AND
		the least (furthest from 0) negative value.

		For every cell in the given layer, if the value is currently positive divide it by the
		greatest positive value, or if the value is negative divide it by -1.0 * the least negative value
		(so that it remains a negative number).  This will keep the values in the range of [-1, 1].
	*/

	// WRITE YOUR CODE HERE
}

void enemy_field_of_view(MapLayer<float>& layer, float fovAngle, float closeDistance, float occupancyValue, AStarAgent* enemy)
{
	/*
		First, clear out the old values in the map layer by setting any negative value to 0.
		Then, for every cell in the layer that is within the field of view cone, from the
		enemy agent, mark it with the occupancy value.  Take the dot product between the view
		vector and the vector from the agent to the cell, both normalized, and compare the
		cosines directly instead of taking the arccosine to avoid introducing floating-point
		inaccuracy (larger cosine means smaller angle).

		If the tile is close enough to the enemy (less than closeDistance),
		you only check if it's visible to enemy.  Make use of the is_clear_path
		helper function.  Otherwise, you must consider the direction the enemy is facing too.
		This creates a radius around the enemy that the player can be detected within, as well
		as a fov cone.
	*/

	// WRITE YOUR CODE HERE
}

bool enemy_find_player(MapLayer<float>& layer, AStarAgent* enemy, Agent* player)
{
	/*
		Check if the player's current tile has a negative value, ie in the fov cone
		or within a detection radius.
	*/

	const auto& playerWorldPos = player->get_position();

	const auto playerGridPos = terrain->get_grid_position(playerWorldPos);

	// verify a valid position was returned
	if (terrain->is_valid_grid_position(playerGridPos) == true)
	{
		if (layer.get_value(playerGridPos) < 0.0f)
		{
			return true;
		}
	}

	// player isn't in the detection radius or fov cone, OR somehow off the map
	return false;
}

bool enemy_seek_player(MapLayer<float>& layer, AStarAgent* enemy)
{
	/*
		Attempt to find a cell with the highest nonzero value (normalization may
		not produce exactly 1.0 due to floating point error), and then set it as
		the new target, using enemy->path_to.

		If there are multiple cells with the same highest value, then pick the
		cell closest to the enemy.

		Return whether a target cell was found.
	*/

	// WRITE YOUR CODE HERE

	return false; // REPLACE THIS
}
