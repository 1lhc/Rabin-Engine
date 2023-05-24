#include <pch.h>
#include "L_JumpAway.h"
#include "Agent/BehaviorAgent.h"

void L_JumpAway::on_enter()
{
	// set animation, speed, etc

	//agent->look_at_point(targetPoint);

	BehaviorNode::on_leaf_enter();
}

void L_JumpAway::on_update(float dt)
{
	Vec3 newPosition = { 0.f,0.f,0.f };
	if (agent->get_position().x < 50.f) {
		newPosition.x = agent->get_position().x + 50.f;
	}
	if (agent->get_position().x > 50.f) {
		newPosition.x = agent->get_position().x - 50.f;
	}

	if (agent->get_position().z < 50.f) {
		newPosition.z = agent->get_position().z + 50.f;
	}
	if (agent->get_position().z > 50.f) {
		newPosition.z = agent->get_position().z - 50.f;
	}

	newPosition.y = agent->get_position().y;

	agent->set_position(newPosition);
	on_success();
	display_leaf_text();
}
