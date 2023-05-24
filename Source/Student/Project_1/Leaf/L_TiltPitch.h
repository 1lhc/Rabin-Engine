#pragma once
#include "BehaviorNode.h"
#include "Misc/NiceTypes.h"

class L_TiltPitch : public BaseNode<L_TiltPitch>
{
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;

private:
    float initialPitch;
    float delay;
};