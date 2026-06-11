#ifdef XAPI_USE_V1_0_0
#pragma once
#ifndef XAPI_HEADER
#define XAPI_HEADER

#ifdef _WIN32
#ifdef __cplusplus
#define XAPI_EXPORT __declspec(dllexport) extern "C"
#define XAPI_LOCAL
#else
#define XAPI_EXPORT __declspec(dllexport)
#define XAPI_LOCAL
#endif
#else
#define XAPI_EXPORT __attribute__((visibilty("default")))
#define XAPI_LOCAL __attribute__((visibilty("hidden")))
#endif
#define XAPI_STDCALL __stdcall

#define XAPI_VERSION 0x01000000u
#define XAPI_NULL (Xvoid*)0

#define XE_ERROR 0xBADC0DE

#define XE_EVENT_TYPE_LOG 0xA2942192B2001D9E //velo.event.log

#define XE_EVENT_LOG_DEBUG 0x08

#define Xconst const
#define Xconstptr const*
#define XInterface struct

#ifdef __cplusplus
extern "C" {
#endif

	typedef char Xint8;
	typedef short Xint16;
	typedef int Xint32;
	typedef long long Xint64;

	typedef unsigned char Xuint8;
	typedef unsigned short Xuint16;
	typedef unsigned int Xuint32;
	typedef unsigned long long Xuint64;

	typedef Xconst char Xconstptr Xcstrcp;
	typedef Xconst char* Xcstr;

	typedef void Xvoid;

	typedef XInterface XIDevice XIDevice;
	typedef XInterface XIContext XIContext;
	typedef Xconst Xint32(XAPI_STDCALL* XI_createDeviceAndContextFn)(XIDevice**, XIContext**);
	typedef Xconst Xint32(XAPI_STDCALL* XI_destroyDeviceAndContextFn)(XIDevice**, XIContext**);

	typedef XInterface {
		Xcstrcp name;
		Xcstrcp author;
		Xint32 version;
		Xcstrcp language;
		Xcstrcp sdkName;
		Xcstrcp dependencies;
	}XAPIDescriptor;

	typedef XInterface {
		Xint64 unused;
	}XHQEventBus;

	typedef XInterface{
		Xcstr name;
		Xuint16 maxSize;
	}XQEventBusDescriptor;

	typedef struct {
		Xcstr channel;
		Xcstr msg;
		Xuint8 severity;
	}XSEventLog;

	typedef XInterface {
		Xint32(*onShutdown)(Xvoid*);
		Xint32(*onUpdate)(Xvoid*);
		Xint32(*onInit)(Xvoid*);
	}XIExports;

	typedef XInterface {
		Xvoid(*addRef)(XIDevice* device);
		Xvoid(*release)(XIDevice* device);
		Xuint32(*createQEventBus)(XIDevice* device, XHQEventBus* qBus, XQEventBusDescriptor desc);
		Xvoid(*setXIExport)(XIDevice* device, XIExports exp);
	}XIDeviceVTable;

	XInterface XIDevice {
		Xconst XIDeviceVTable Xconstptr vtbl;
	};

	typedef XInterface {
		Xvoid(*addRef)(XIContext* ctx);
		Xvoid(*release)(XIContext* ctx);
		Xuint32(*postEvent)(XHQEventBus* qBus, Xvoid* event, Xconst Xuint64 type);
		Xuint32(*subscribeEvent)(XHQEventBus* qBus, Xconst Xuint64 type, Xconst Xvoid Xconstptr fn);
	}XIContextVTable;

	XInterface XIContext{
		Xconst XIContextVTable Xconstptr vtbl;
	};
#ifdef __cplusplus
}
#endif
#endif
#endif
