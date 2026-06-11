#define XAPI_USE_V1_0_0
#include "XVAPI.h"

XAPIDescriptor desc;
XIDevice* device;
XIContext* ctx;
XI_createDeviceAndContextFn XI_createDeviceAndContext;
XI_destroyDeviceAndContextFn XI_destroyDeviceAndContext;
XHQEventBus qLogBus;

XAPI_EXPORT XAPIDescriptor XI_query(Xvoid) {
	desc.author = "CDevJoud";
	desc.dependencies = "xapi.sys.q_event_bus;";
	desc.language = "C";
	desc.name = "MyPlugin";
	desc.sdkName = "vanilla";
	desc.version = XAPI_VERSION;
	return desc;
}

XAPI_EXPORT Xint32 XI_main(Xvoid* pParam) {
	XI_createDeviceAndContext = pParam;
	Xint32 ret = XI_createDeviceAndContext(&device, &ctx);

	if (ret != XE_ERROR) {
		if (device == XAPI_NULL || ctx == XAPI_NULL) {
			return XE_ERROR;
		}

		if (device->vtbl == XAPI_NULL || ctx->vtbl == XAPI_NULL) {
			return XE_ERROR;
		}

		if (device->vtbl->createQEventBus == XAPI_NULL || ctx->vtbl->postEvent == XAPI_NULL) {
			return XE_ERROR;
		}

		XQEventBusDescriptor qLogBusDesc;
		qLogBusDesc.name = "LCEServerLog";
		qLogBusDesc.maxSize = 0xFFFF;
		ret = device->vtbl->createQEventBus(&device, &qLogBus, qLogBusDesc);

		if (ret == XE_ERROR) {
			return XE_ERROR;
		}

		XSEventLog log;
		log.channel = "default";
		log.msg = "Hello From vanilla C!";
		log.severity = XE_EVENT_LOG_DEBUG;

		ctx->vtbl->postEvent(&qLogBus, &log, XE_EVENT_TYPE_LOG);

	}
	return 1;
}

XAPI_EXPORT Xint32 XI_terminate(Xvoid* pParam) {
	device->vtbl->release(&device);
	ctx->vtbl->release(&ctx);
	XI_destroyDeviceAndContext = pParam;
	XI_destroyDeviceAndContext(&device, &ctx);
	return 1;
}
