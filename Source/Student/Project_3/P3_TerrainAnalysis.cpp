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


				/*	if (terrain->is_valid_grid_position(r + 1, c) && !terrain->is_wall(r + 1, c) && layer.get_value(r + 1, c) == 0.0f) {
						layer.set_value(r + 1, c, 0.5f);
					}
					if (terrain->is_valid_grid_position(r, c - 1) && !terrain->is_wall(r, c - 1) && layer.get_value(r, c - 1) == 0.0f) {
						layer.set_value(r, c - 1, 0.5f);
					}
					if (terrain->is_valid_grid_position(r, c + 1) && !terrain->is_wall(r, c + 1) && layer.get_value(r, c + 1) == 0.0f) {
						layer.set_value(r, c + 1, 0.5f);
					}
					if (terrain->is_valid_grid_position(r - 1, c) && !terrain->is_wall(r - 1, c) && layer.get_value(r - 1, c) == 0.0f) {
						layer.set_value(r - 1, c, 0.5f);
					}
					bool topLeftwallexists = false, topRightwallexists = false, bottomLeftwallexists = false, bottomRightwallexists = false;

					if (terrain->is_valid_grid_position(r + 1, c) && terrain->is_wall(r + 1, c) &&
						terrain->is_valid_grid_position(r, c - 1) && terrain->is_wall(r, c - 1)) {
						topLeftwallexists = true;
					}

					if (terrain->is_valid_grid_position(r + 1, c) && terrain->is_wall(r + 1, c) &&
						terrain->is_valid_grid_position(r, c + 1) && terrain->is_wall(r, c + 1)) {
						topRightwallexists = true;
					}

					if (terrain->is_valid_grid_position(r - 1, c) && terrain->is_wall(r - 1, c) &&
						terrain->is_valid_grid_position(r, c - 1) && terrain->is_wall(r, c - 1)) {
						bottomLeftwallexists = true;
					}

					if (terrain->is_valid_grid_position(r - 1, c) && terrain->is_wall(r - 1, c) &&
						terrain->is_valid_grid_position(r, c + 1) && terrain->is_wall(r, c + 1)) {
						bottomRightwallexists = true;
					}
					if (topLeftwallexists == false) {
						if (terrain->is_valid_grid_position(r + 1, c - 1) && !terrain->is_wall(r + 1, c - 1) && layer.get_value(r + 1, c - 1) == 0.0f)
							layer.set_value(r + 1, c - 1, 0.5f);
					}

					if (topRightwallexists == false) {
						if (terrain->is_valid_grid_position(r + 1, c + 1) && !terrain->is_wall(r + 1, c + 1) && layer.get_value(r + 1, c + 1) == 0.0f)
							layer.set_value(r + 1, c + 1, 0.5f);
					}

					if (bottomLeftwallexists == false) {
						if (terrain->is_valid_grid_position(r - 1, c - 1) && !terrain->is_wall(r - 1, c - 1) && layer.get_value(r - 1, c - 1) == 0.0f)
							layer.set_value(r - 1, c - 1, 0.5f);
					}
					if (bottomRightwallexists == false) {
						if (terrain->is_valid_grid_position(r - 1, c + 1) && !terrain->is_wall(r - 1, c + 1) && layer.get_value(r - 1, c + 1) == 0.0f)
							layer.set_value(r - 1, c + 1, 0.5f);
					}*/
				}
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
	// cos titer =AB/|A||B|
	// WRITE YOUR CODE HERE



	//// Convert agent's position to 2D coordinates
	///*int agentRow = static_cast<int>(agent->get_position().y);
	//int agentCol = static_cast<int>(agent->get_position().x);*/

	//
	//// Calculate agent's view direction vector
	///*float agentDirectionX = std::cos(agent->get_yaw());
	//float agentDirectionZ = std::sin(agent->get_yaw());*/
	//float agentDirectionX = static_cast<float>(std::cos(terrain->get_grid_position(agent->get_forward_vector()).col));
	//float agentDirectionZ = static_cast<float>(std::sin(terrain->get_grid_position(agent->get_forward_vector()).row));

	//// Calculate field of view boundaries
	//const float fieldOfView = 180.0f;  // slightly larger than 180 degrees
	//const float halfFieldOfView = fieldOfView / 2.0f;
	//float minCosine = static_cast<float>(std::cos(halfFieldOfView * (M_PI / 180.0f)));

	//// Iterate over all cells in the layer
	//for (int row = 0; row < layerHeight; ++row)
	//{
	//	for (int col = 0; col < layerWidth; ++col)
	//	{
	//		// Calculate the vector from agent to cell
	//		/*float cellVectorX = col + 0.5f - agent->get_position().x;
	//		float cellVectorZ = row + 0.5f - agent->get_position().z;*/
	//		int cellVectorX = col /*+ 0.5f*/ - terrain->get_grid_position(agent->get_position()).col;
	//		int cellVectorZ = row /*+ 0.5f */- terrain->get_grid_position(agent->get_position()).row;

	//		// Normalize the cell vector
	//		//float cellVectorLength = std::sqrt(cellVectorX * cellVectorX + cellVectorZ * cellVectorZ);
	//		/*cellVectorX /= cellVectorLength;
	//		cellVectorZ /= cellVectorLength;*/

	//		// Calculate the dot product between view vector and cell vector
	//		float dotProduct = agentDirectionX * cellVectorX + agentDirectionZ * cellVectorZ;

	//		// Check if the cell is within agent's field of view
	//		if (dotProduct >= minCosine && is_clear_path(agentRow, agentCol, row, col))
	//		{
	//			// Mark the cell as visible
	//			layer.set_value(row, col, 1.0f);
	//		}
	//	}
	//}
	const int layerHeight = terrain->get_map_height();
	const int layerWidth = terrain->get_map_width();
	int agentRow = terrain->get_grid_position(agent->get_position()).row;
	int agentCol = terrain->get_grid_position(agent->get_position()).col;
	Vec3 agentView = agent->get_forward_vector();
	//float fieldofview = static_cast<float>(std::cos((180.f * M_PI / 180.f) / 2));
	for (int row = 0; row < layerHeight; ++row)
	{
		for (int col = 0; col < layerWidth; ++col)
		{
			// Calculate the vector from agent to cell
			Vec3 AgentToCell = terrain->get_world_position(row, col) - agent->get_position();
			float costiter = (AgentToCell.Dot(agentView));// / (AgentToCell.Length() * agentView.Length());

			// Check if the cell is within agent's field of view
			if (costiter >= std::cosf(PI / 1.99f) && is_clear_path(agentRow, agentCol, row, col))
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
	 // Step 1: Initialize a temporary layer with the same dimensions as the given layer
	float tempLayer[40][40];
	for (int i = 0; i < 40; ++i) {
		for (int j = 0; j < 40; ++j) {
			tempLayer[i][j] = 0.f; // Initialize with current values
		}
	}

	// Step 2: Process each cell in the given layer
	for (int i = 0; i < terrain->get_map_height(); ++i) {
		for (int j = 0; j < terrain->get_map_width(); ++j) {
			// Step 1: Get the value of each neighbor and apply decay factor
			float maxNeighborValue = 0.0;
			for (int dx = -1; dx <= 1; dx++) {
				for (int dy = -1; dy <= 1; dy++) {
					if (dx == 0 && dy == 0) continue; // Skip the current cell
					int nx = i + dx;
					int ny = j + dy;
					if (terrain->is_valid_grid_position(nx,ny) && !terrain->is_wall(nx,ny)) {
						float distance = static_cast<float>(sqrt(pow(i - nx, 2) + pow(j - ny, 2)));
						if (terrain->is_valid_grid_position(nx, ny)) {
							float neighborValue = layer.get_value(nx, ny) * exp(-1 * distance * decay);
							if (neighborValue > maxNeighborValue) {
								maxNeighborValue = neighborValue;
							}
						}
					}
				}
			}

			// Step 3: Linearly interpolate from the cell's current value to the value from step 2
			float currentValue = layer.get_value(i, j);
			float newValue = lerp(currentValue, maxNeighborValue, growth);

			// Step 4: Store the value from step 3 in the temporary layer
			tempLayer[i][j] = newValue;
		}
	}

	// Step 5: Write the temporary layer into the given layer
	for (int i = 0; i < terrain->get_map_height(); i++) {
		for (int j = 0; j < terrain->get_map_width(); j++) {
			if (terrain->is_valid_grid_position(i, j) && !terrain->is_wall(i, j)) {
				layer.set_value(i, j, tempLayer[i][j]);
			}
		}
	}
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
	 // Step 1: Initialize a temporary layer with the same dimensions as the given layer
	float tempLayer[40][40];
	for (int i = 0; i < 40; i++) {
		for (int j = 0; j < 40; j++) {
			tempLayer[i][j] = layer.get_value(i, j); // Initialize with current values
		}
	}

	// Step 2: Process each cell in the given layer
	for (int i = 0; i < 40; i++) {
		for (int j = 0; j < 40; j++) {
			// Step 1: Get the value of each neighbor and apply decay factor
			float maxNeighborValue = 0.0;
			for (int dx = -1; dx <= 1; dx++) {
				for (int dy = -1; dy <= 1; dy++) {
					if (dx == 0 && dy == 0) continue; // Skip the current cell
					int nx = i + dx;
					int ny = j + dy;
					if (nx >= 0 && nx < 40 && ny >= 0 && ny < 40) {
						float neighborValue = layer.get_value(nx, ny) * decay;
						if (std::abs(neighborValue) > std::abs(maxNeighborValue)) {
							maxNeighborValue = neighborValue;
						}
					}
				}
			}

			// Step 3: Linearly interpolate from the cell's current value to the value from step 2
			float currentValue = layer.get_value(i, j);
			float newValue = lerp(currentValue, maxNeighborValue, growth);

			// Step 4: Store the value from step 3 in the temporary layer
			tempLayer[i][j] = newValue;
		}
	}

	// Step 5: Write the temporary layer into the given layer
	for (int i = 0; i < 40; i++) {
		for (int j = 0; j < 40; j++) {
			layer.set_value(i, j, tempLayer[i][j]);
		}
	}
}

void normalize_solo_occupancy(MapLayer<float>& layer)
{
	/*
		Determine the maximum value in the given layer, and then divide the value
		for every cell in the layer by that amount.  This will keep the values in the
		range of [0, 1].  Negative values should be left unmodified.
	*/

	// WRITE YOUR CODE HERE
	 // Step 1: Find the maximum value in the given layer
	float maxValue = std::numeric_limits<float>::min();
	for (int i = 0; i < terrain->get_map_height(); i++) {
		for (int j = 0; j < terrain->get_map_width(); j++) {
			float value = layer.get_value(i, j);
			if (value > maxValue) {
				maxValue = value;
			}
		}
	}

	// Step 2: Normalize the values in the layer
	for (int i = 0; i < terrain->get_map_height(); i++) {
		for (int j = 0; j < terrain->get_map_width(); j++) {
			float value = layer.get_value(i, j);
			if (value >= 0) {
				float normalizedValue = value / maxValue;
				layer.set_value(i, j, normalizedValue);
			}
		}
	}
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
	 // Clear out the old values in the map layer by setting any negative value to 0.
	for (int i = 0; i < terrain->get_map_width(); i++)
	{
		for (int j = 0; j < terrain->get_map_height(); j++)
		{
			if (layer.get_value(i, j) < 0)
				layer.set_value(i, j, 0.f);
		}
	}

	const int layerHeight = terrain->get_map_height();
	const int layerWidth = terrain->get_map_width();
	int enemyRow = terrain->get_grid_position(enemy->get_position()).row;
	int enemyCol = terrain->get_grid_position(enemy->get_position()).col;
	Vec3 enemyView = enemy->get_forward_vector();
	float fieldofview = static_cast<float>(std::cos((180.f * M_PI / 180.f) / 2));
	for (int row = 0; row < layerHeight; ++row)
	{
		for (int col = 0; col < layerWidth; ++col)
		{
			// Calculate the vector from agent to cell
			Vec3 AgentToCell = terrain->get_world_position(row, col) - enemy->get_position();
			float costiter = (AgentToCell.Dot(enemyView)) / (AgentToCell.Length() * enemyView.Length());
			if (AgentToCell.Length() < closeDistance) {
				if (is_clear_path(enemyRow, enemyCol, row, col))
				{
					// Mark the cell as visible
					layer.set_value(row, col, occupancyValue);
				}
			}
			// Check if the cell is within agent's field of view
			else if (costiter > std::cosf(fovAngle / 2) && is_clear_path(enemyRow, enemyCol, row, col))
			{
				// Mark the cell as visible
				layer.set_value(row, col, occupancyValue);
			}
		}
	}

	// Calculate the view vector from the enemy agent.
	/*Vector2D enemyPos = enemy->getPosition();
	Vector2D enemyFacing = enemy->getFacingDirection();*/
	//Vec3 enemyPos = enemy->get_position();
	//Vec3 enemyFacing = enemy->get_forward_vector();

	//GridPos ePos = terrain->get_grid_position(enemyPos);
	//// Iterate over every cell in the layer.
	//for (int i = 0; i < terrain->get_map_width(); i++)
	//{
	//	for (int j = 0; j < terrain->get_map_height(); j++)
	//	{
	//		// Get the vector from the enemy to the current cell.
	//		Vec3 cellPos = { static_cast<float>(j), 0.f, static_cast<float>(i) }; // MAYBE WRONG
	//		Vec3 toCell = cellPos - enemyPos;

	//		// Calculate the dot product between the view vector and the vector to the cell.
	//		//float dotProduct = Vector2D::dot(enemyFacing.normalized(), toCell.normalized());
	//		float dotProduct = enemyFacing.Dot(toCell);


	//		// Compare the cosines to check if the cell is within the field of view cone.
	//		if (dotProduct >= cosf(fovAngle / 2.0f))
	//		{
	//			// Check if the cell is close enough to the enemy.
	//			if (toCell.Length() <= closeDistance)
	//			{
	//				// Check if the cell is visible to the enemy.
	//				if (is_clear_path(ePos.row, ePos.col, i, j))
	//					/*layer(i, j) = occupancyValue;*/
	//					layer.set_value(i, j, occupancyValue);
	//			}
	//			else
	//			{
	//				// Mark the cell with the occupancy value.
	//				// layer(i, j) = occupancyValue;
	//				layer.set_value(i, j, occupancyValue);
	//			}
	//		}
	//	}
	//}
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

	//return false; // REPLACE THIS
	// Initialize variables to track the highest value and its position.
	float highestValue = 0.0f;
	Vec3 targetCell;

	// Iterate over every cell in the layer.
	for (int i = 0; i < terrain->get_map_width(); i++)
	{
		for (int j = 0; j < terrain->get_map_height(); j++)
		{
			float cellValue = layer.get_value(i, j);
			// Check if the cell value is higher than the current highest value.
			if (cellValue > highestValue)
			{
				highestValue = cellValue;
				targetCell = Vec3(static_cast<float>(j), 0.f, static_cast<float>(i));
			}
			// If there are multiple cells with the same highest value, pick the closest one.
			else if (cellValue == highestValue && targetCell.Length() > Vec3(static_cast<float>(j), 0.f, static_cast<float>(i)).Length())
			{
				targetCell = Vec3(static_cast<float>(j), 0.f, static_cast<float>(i));
			}
		}
	}

	// Check if a target cell was found.
	if (highestValue > 0.0f)
	{
		// Set the target cell as the new target for the enemy agent.
		enemy->path_to(targetCell);
		return true;
	}

	return false;
}
