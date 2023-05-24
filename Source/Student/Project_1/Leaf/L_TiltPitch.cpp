#include <pch.h>
#include "L_TiltPitch.h"
#include "Agent/BehaviorAgent.h"

void L_TiltPitch::on_enter()
{
	// set animation, speed, etc

	//targetScale = RNG::range(0.5f, 5.f);
	initialPitch = agent->get_pitch();
	delay = 5.f;
	//agent->look_at_point(targetPoint);

	BehaviorNode::on_leaf_enter();
}

void L_TiltPitch::on_update(float dt)
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

	agent->set_pitch(initialPitch - 0.785398f);
	//set_status(NodeStatus::RUNNING);

	if (delay < 0.0f)
	{
		agent->set_pitch(initialPitch);

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
