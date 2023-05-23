#pragma once
#include "BehaviorNode.h"

class D_AlwaysTrue : public BaseNode<D_AlwaysTrue>
{
protected:
    virtual void on_update(float dt) override;
    virtual void on_exit() override;
};