#pragma once
#include "BehaviorNode.h"
#include "Misc/NiceTypes.h"

class L_RotateYaw : public BaseNode<L_RotateYaw>
{
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;

private:
    float initialYaw;
    float increment;
};