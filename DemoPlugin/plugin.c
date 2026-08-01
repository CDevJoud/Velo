#define XAPI_USE_V1_0_0
#include "XVAPI.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

XAPIDescriptor desc;
XIDevice* device;
XIContext* ctx;
Xuint64 session = 0;
XI_createDeviceAndContextFn XI_createDeviceAndContext;
XI_destroyDeviceAndContextFn XI_destroyDeviceAndContext;
XHQEventBus qLogBus, qBus;

XAPI_LOCAL Xvoid LogDebug(Xcstrcp msg) {
	XSEventLog log;
	log.channel = "default";
	log.msg = msg;
	log.severity = XE_EVENT_LOG_DEBUG;
	ctx->vtbl->postEvent(qLogBus, &log, XE_EVENT_TYPE_LOG);
}

XAPI_LOCAL Xuint32 cb_player_connect(Xvoid* e) {
	XSEventClientConnect* ecc = e;

	XSEventLog log;
	log.channel = "default";
	Xint8 buf[128];
	memset(buf, 0, 128);
	// \xff indicates that the string is heap allocated and need to be freed when its not used!
	snprintf(buf, 128, "From DemoPlugin! client %s:%d connected!\xff", ecc->remoteAddress, ecc->remotePort);
	const size_t len = strlen(buf);

	Xint8 _buf = ctx->vtbl->memAlloc(128);
	snprintf(_buf, 128, "From DemoPlugin! client %s:%d connected!\xff", ecc->remoteAddress, ecc->remotePort);

	LogDebug(_buf);
	return 1;
}

XAPI_EXPORT XAPIDescriptor XI_query(Xvoid) {
	desc.author = "CDevJoud";
	desc.dependencies = "xapi.sys.q_event_bus;";
	desc.language = "C";
	desc.name = "MyPlugin";
	desc.sdkName = "vanilla";
	desc.version = XAPI_VERSION;
	desc.session = &session;
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

		XQEventBusDescriptor qBusDesc;
		qBusDesc.name = "LCEServer";
		qBusDesc.maxSize = 0xFFFF;
		ret = device->vtbl->createQEventBus(&device, &qBus, qBusDesc);

		if (ret == XE_ERROR) {
			return XE_ERROR;
		}

		XSEventLog log;
		log.channel = "default";
		log.msg = "Hello From vanilla C!";
		log.severity = XE_EVENT_LOG_DEBUG;

		ctx->vtbl->postEvent(qLogBus, &log, XE_EVENT_TYPE_LOG);

		ctx->vtbl->subscribeEvent(qBus, XE_EVENT_TYPE_CLIENT_CONNECT, cb_player_connect);
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
