#include <pch.h>
#include "D_RepeatTwoTimes.h"

D_RepeatTwoTimes::D_RepeatTwoTimes() : counter(0)
{}

void D_RepeatTwoTimes::on_enter()
{
    counter = 0;
    BehaviorNode::on_enter();
}

void D_RepeatTwoTimes::on_update(float dt)
{
    // repeat the child until 2 successes, stop on any failure
    BehaviorNode *child = children.front();

    child->tick(dt);

    if (child->succeeded() == true)
    {
        ++counter;

        if (counter == 2)
        {
            on_success();
        }
        else
        {
            child->set_status(NodeStatus::READY);
        }
    }
    else if (child->failed() == true)
    {
        on_failure();
    }
}
