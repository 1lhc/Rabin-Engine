#pragma once
#include "BehaviorNode.h"
#include "Misc/NiceTypes.h"

class L_ScaleRandomly : public BaseNode<L_ScaleRandomly>
{
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;

private:
    float targetScale;
};