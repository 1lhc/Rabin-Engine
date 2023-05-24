#pragma once
#include "BehaviorNode.h"
#include "Misc/NiceTypes.h"

class L_ScaleUp : public BaseNode<L_ScaleUp>
{
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;

private:
    Vec3 initialScale;
    float delay;
};