#pragma once
#include "BehaviorNode.h"

class D_RepeatTwoTimes : public BaseNode<D_RepeatTwoTimes>
{
public:
    D_RepeatTwoTimes();

protected:
    unsigned counter;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};