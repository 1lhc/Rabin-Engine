#pragma once
#include "BehaviorNode.h"
#include "Misc/NiceTypes.h"

class L_SpeedUp : public BaseNode<L_SpeedUp>
{
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;

private:
    float initialSpeed;
    float delay;
};