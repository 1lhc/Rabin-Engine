#pragma once
#include "BehaviorNode.h"
#include "Misc/NiceTypes.h"

class L_ChangeToRandomColor : public BaseNode<L_ChangeToRandomColor>
{
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;

private:
    Vec3 targetColor;
};