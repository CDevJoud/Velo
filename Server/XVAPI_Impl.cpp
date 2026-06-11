#include "XVAPI_Impl.hpp"

std::unordered_map<Xuint64, Xvoid*> registries;

velo::QEventBus* _qLog = nullptr;
Xuint64 hash_str(Xcstr str) {
	Xuint64 h = 0xcbf29ce484222325ULL; // FNV offset basis

	while (*str) {
		h ^= (Xuint8)(*str++);
		h *= 0x100000001b3ULL; // FNV prime
	}

	// Final mixing (improves avalanche)
	h ^= h >> 33;
	h *= 0xff51afd7ed558ccdULL;
	h ^= h >> 33;
	h *= 0xc4ceb9fe1a85ec53ULL;
	h ^= h >> 33;

	return h;
}

typedef struct {
	XIDevice iface;
	Xint32 ref;
	Xint32 handle;
}DeviceImpl;

typedef struct {
	XIContext iface;
	Xint32 ref;
	Xint32 handle;
}ContextImpl;

Xvoid velo_device_addRef(XIDevice* device) {
	DeviceImpl& self = (*(DeviceImpl*)device->vtbl);
	self.ref++;
}

Xvoid velo_device_release(XIDevice* device) {
	DeviceImpl& self = (*(DeviceImpl*)device->vtbl);
	if (--self.ref == 0) {
		//log that device destroyed;
		_qLog->post(velo::event::Log(velo::event::Log::DEBUG, "device released!"));
	}
}

Xvoid velo_ctx_addRef(XIContext* ctx) {
	ContextImpl& self = (*(ContextImpl*)ctx->vtbl);
	self.ref++;
}

Xvoid velo_ctx_release(XIContext* ctx) {
	ContextImpl& self = (*(ContextImpl*)ctx->vtbl);
	if (--self.ref == 0) {
		//log that context destroyed;
		_qLog->post(velo::event::Log(velo::event::Log::DEBUG, "context released!"));
	}
}

Xuint32 velo_device_createQEventBus(XIDevice* device, XHQEventBus* qBus, XQEventBusDescriptor desc) {
	Xuint64 hash = hash_str(desc.name);
	auto it = registries.find(hash);
	if (it == registries.end()) {
		registries[hash] = new velo::QEventBus(desc.name);
		qBus->unused = hash;
		return 1;
	}
	else {
		qBus->unused = it->first;
	}
	return 1;
}

Xuint32 velo_ctx_postEvent(XHQEventBus* qBus, Xvoid* event, Xconst Xuint64 type) {
	auto it = registries.find(qBus->unused);
	if (it != registries.end()) {
		velo::QEventBus* _qBus = reinterpret_cast<velo::QEventBus*>(it->second);

		if (type == XE_EVENT_TYPE_LOG) {
			XSEventLog* XAPI_eventLog = reinterpret_cast<XSEventLog*>(event);
			velo::event::Log log;
			log.channel = XAPI_eventLog->channel;
			log.msg = XAPI_eventLog->msg;
			log.severity = static_cast<velo::event::Log::Severity>(XAPI_eventLog->severity);
			_qBus->post(log);
			return 1;
		}
	}
	return XE_ERROR;
}

Xvoid pushQEventBus(velo::QEventBus* qBus) {
	Xuint64 hash = hash_str(qBus->getNamespace().data());
	auto it = registries.find(hash);
	if (it == registries.end()) {
		registries[hash] = qBus;
	}
}

void setGlobalEventBus(velo::QEventBus* qBus) {
	_qLog = qBus;
}

Xint32 velo_createDeviceAndContext(XIDevice** device, XIContext** ctx) {
	if ((*device) != nullptr && (*ctx) != nullptr) {
		return XE_ERROR;
	}

	DeviceImpl* deviceImpl = new DeviceImpl();
	ContextImpl* ctxImpl = new ContextImpl();

	deviceImpl->ref++;
	ctxImpl->ref++;

	XIDeviceVTable* device_vtbl = new XIDeviceVTable();
	XIContextVTable* ctx_vtbl = new XIContextVTable();

	if (!device_vtbl && !ctx_vtbl) {
		return XE_ERROR;
	}

	const_cast<XIDeviceVTable*>(device_vtbl)->addRef = velo_device_addRef;
	const_cast<XIDeviceVTable*>(device_vtbl)->release = velo_device_release;
	const_cast<XIDeviceVTable*>(device_vtbl)->createQEventBus = velo_device_createQEventBus;

	const_cast<XIContextVTable*>(ctx_vtbl)->addRef = velo_ctx_addRef;
	const_cast<XIContextVTable*>(ctx_vtbl)->release = velo_ctx_release;
	const_cast<XIContextVTable*>(ctx_vtbl)->postEvent = velo_ctx_postEvent;

	deviceImpl->iface.vtbl = device_vtbl;
	ctxImpl->iface.vtbl = ctx_vtbl;

	*device = (XIDevice*)deviceImpl;
	*ctx = (XIContext*)ctxImpl;
	
	return 1;
}

Xint32 velo_destroyDeviceAndContext(XIDevice** device, XIContext** ctx) {
	if ((*device) == nullptr && (*ctx) == nullptr) {
		return XE_ERROR;
	}

	DeviceImpl* deviceImpl = (DeviceImpl*)(*device);
	ContextImpl* ctxImpl = (ContextImpl*)(*ctx);
	do {
		// wait until the ref counter is zero
	} while (deviceImpl->ref != 0 && ctxImpl->ref != 0);


	delete deviceImpl->iface.vtbl;
	delete ctxImpl->iface.vtbl;

	delete deviceImpl;
	delete ctxImpl;

	deviceImpl = nullptr;
	ctxImpl = nullptr;
	return 1;
}
