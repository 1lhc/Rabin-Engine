#include <pch.h>
#include "L_RotateYaw.h"
#include "Agent/BehaviorAgent.h"

void L_RotateYaw::on_enter()
{
	// set animation, speed, etc

	//targetScale = RNG::range(0.5f, 5.f);
	initialYaw = agent->get_yaw();
	increment = 0.f;
	//agent->look_at_point(targetPoint);

	BehaviorNode::on_leaf_enter();
}

void L_RotateYaw::on_update(float dt)
{
	//const auto result = agent->move_toward_point(targetPoint, dt);
	/*const auto result = */
	/*agent->set_color(targetColor);*/
	//for (float timer = 0.f, float increment = 1.f ; timer <= 5.f; timer += dt, ++increment) {
	//	agent->set_yaw(initialYaw + increment);
	//}
	//float increment = 1.f;
	/*for (float timer = 0.f; timer <= 5.f; timer += dt) {
		agent->set_yaw(initialYaw + increment);
		++increment;
	}*/

	float total = initialYaw + increment;
	agent->set_yaw(total);
	increment += 0.0174533f;
	set_status(NodeStatus::RUNNING);
	//agent->set_scaling(targetScale);

	/*  if (result == true)
	  {*/
	if (increment >= 6.28319f) {
		on_success();
	}
	//}

	display_leaf_text();
}
