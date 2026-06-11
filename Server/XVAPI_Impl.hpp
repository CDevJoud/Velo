#pragma once
#define XAPI_USE_V1_0_0
#include "XVAPI.h"
#include "QEventBus.hpp"

Xvoid pushQEventBus(velo::QEventBus* qBus);
void setGlobalEventBus(velo::QEventBus* qBus);


Xint32 velo_createDeviceAndContext(XIDevice** device, XIContext** ctx);
Xint32 velo_destroyDeviceAndContext(XIDevice** device, XIContext** ctx);
