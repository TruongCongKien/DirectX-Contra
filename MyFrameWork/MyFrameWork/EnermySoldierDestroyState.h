#pragma once
#include "EnermyState.h"
#include "EnermyData.h"
#include "EnermySoldierData.h"

class EnermySoldierDestroyState : public EnermyState
{
public:
    EnermySoldierDestroyState(EnermyData *pData);
    ~EnermySoldierDestroyState();

    void onUpdate();
};

