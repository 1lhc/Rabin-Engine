#pragma once
#include "BehaviorNode.h"
#include "Misc/NiceTypes.h"

class L_JumpAway : public BaseNode<L_JumpAway>
{
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;

private:
    
};