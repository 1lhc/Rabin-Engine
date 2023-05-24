#pragma once
#include "BehaviorNode.h"

class D_DelayFiveSeconds : public BaseNode<D_DelayFiveSeconds>
{
public:
    D_DelayFiveSeconds();

protected:
    float delay;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};