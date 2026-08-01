#pragma once
#define XAPI_USE_V1_0_0
#include "XVAPI.h"
#include "QEventBus.hpp"

namespace velo {
	Xvoid pushQEventBus(velo::QEventBus* qBus);
	void setGlobalEventBus(velo::QEventBus* qBus);


	Xint32 createDeviceAndContext(XIDevice** device, XIContext** ctx);
	Xint32 destroyDeviceAndContext(XIDevice** device, XIContext** ctx);
}
