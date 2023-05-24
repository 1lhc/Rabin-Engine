#include <pch.h>
#include "L_ScaleRandomly.h"
#include "Agent/BehaviorAgent.h"

void L_ScaleRandomly::on_enter()
{
	// set animation, speed, etc

	targetScale = RNG::range(0.5f, 5.f);
	//agent->look_at_point(targetPoint);

	BehaviorNode::on_leaf_enter();
}

void L_ScaleRandomly::on_update(float dt)
{
	//const auto result = agent->move_toward_point(targetPoint, dt);
	/*const auto result = */
	/*agent->set_color(targetColor);*/

	agent->set_scaling(targetScale);

	/*  if (result == true)
	  {*/
	on_success();
	//}

	display_leaf_text();
}
