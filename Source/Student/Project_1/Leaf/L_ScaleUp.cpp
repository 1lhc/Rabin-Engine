#include <pch.h>
#include "L_ScaleUp.h"
#include "Agent/BehaviorAgent.h"

void L_ScaleUp::on_enter()
{
	// set animation, speed, etc

	//targetScale = RNG::range(0.5f, 5.f);
	initialScale = agent->get_scaling();
	delay = 5.f;
	//agent->look_at_point(targetPoint);

	BehaviorNode::on_leaf_enter();
}

void L_ScaleUp::on_update(float dt)
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


	//agent->set_scaling(targetScale);

	delay -= dt;

	//agent->set_pitch(initialScale - 0.785398f);
	agent->set_scaling(5.f);
	//set_status(NodeStatus::RUNNING);

	if (delay < 0.0f)
	{
		agent->set_scaling(initialScale);

		on_success();
	}
	/*  if (result == true)
	  {*/
	/*if (increment >= 6.28319f) {
		on_success();
	}*/
	//}

	display_leaf_text();
}
