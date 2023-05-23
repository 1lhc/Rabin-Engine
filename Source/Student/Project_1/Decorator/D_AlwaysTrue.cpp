#include <pch.h>
#include "D_AlwaysTrue.h"

void D_AlwaysTrue::on_update(float dt)
{
    BehaviorNode *child = children.front();

    child->tick(dt);

    if (child->succeeded() == true)
    {
        /*on_failure();*/
        on_success();
    }
    else if (child->failed() == true)
    {
        on_success();
    }

}

void D_AlwaysTrue::on_exit()
{
    // we want the node and child to repeat, so go back to ready
    set_status(NodeStatus::READY);
}

