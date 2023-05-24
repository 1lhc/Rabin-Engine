#include <pch.h>
#include "D_DelayFiveSeconds.h"

D_DelayFiveSeconds::D_DelayFiveSeconds() : delay(0.0f)
{}

void D_DelayFiveSeconds::on_enter()
{
    delay = 5.0f;

    BehaviorNode::on_enter();
}

void D_DelayFiveSeconds::on_update(float dt)
{
    delay -= dt;

    if (delay < 0.0f)
    {
        BehaviorNode *child = children.front();

        child->tick(dt);

        // assume same status as child
        set_status(child->get_status());
        set_result(child->get_result());
    }
}
