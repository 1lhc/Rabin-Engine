#include <pch.h>
#include "L_SpeedUp.h"
#include "Agent/BehaviorAgent.h"

void L_SpeedUp::on_enter()
{
	// set animation, speed, etc

	//targetScale = RNG::range(0.5f, 5.f);
	initialSpeed = agent->get_movement_speed();
	delay = 5.f;
	//agent->look_at_point(targetPoint);

	BehaviorNode::on_leaf_enter();
}

void L_SpeedUp::on_update(float dt)
{
	delay -= dt;
	agent->set_movement_speed(initialSpeed * 2.f);

	if (delay < 0.0f)
	{
		agent->set_movement_speed(initialSpeed);
		on_success();
	}

	display_leaf_text();
}
