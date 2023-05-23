#include <pch.h>
#include "L_ChangeToRandomColor.h"
#include "Agent/BehaviorAgent.h"

void L_ChangeToRandomColor::on_enter()
{
    // set animation, speed, etc

    targetColor = { RNG::range(0.f,1.f), RNG::range(0.f,1.f),RNG::range(0.f,1.f) };
    //agent->look_at_point(targetPoint);

	BehaviorNode::on_leaf_enter();
}

void L_ChangeToRandomColor::on_update(float dt)
{
    //const auto result = agent->move_toward_point(targetPoint, dt);
    /*const auto result = */
        agent->set_color(targetColor);

  /*  if (result == true)
    {*/
        on_success();
    //}

    display_leaf_text();
}
