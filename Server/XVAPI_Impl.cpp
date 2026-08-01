#include "XVAPI_Impl.hpp"
#include "TCPClient.hpp"

namespace velo {
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

	
} // velo

namespace velo {
	namespace device {
		Xvoid addRef(XIDevice* device) {
			DeviceImpl& self = (*(DeviceImpl*)device->vtbl);
			self.ref++;
		}
		Xvoid release(XIDevice* device) {
			DeviceImpl& self = (*(DeviceImpl*)device->vtbl);
			if (--self.ref == 0) {
				_qLog->post(event::Log(event::Log::DEBUG, "device released!"));
			}
		}
		Xuint32 createQEventBus(XIDevice* device, XHQEventBus* qBus, XQEventBusDescriptor desc) {
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
	}// device

	namespace ctx {
		Xvoid addRef(XIContext* ctx) {
			ContextImpl& self = (*(ContextImpl*)ctx->vtbl);
			self.ref++;
		}

		Xvoid release(XIContext* ctx) {
			ContextImpl& self = (*(ContextImpl*)ctx->vtbl);
			if (--self.ref == 0) {
				//log that context destroyed;
				_qLog->post(velo::event::Log(velo::event::Log::DEBUG, "context released!"));
			}
		}
		Xuint32 postEvent(XHQEventBus qBus, Xvoid* event, Xconst Xuint64 type) {
			auto it = registries.find(qBus.unused);
			if (it != registries.end()) {
				velo::QEventBus* _qBus = reinterpret_cast<velo::QEventBus*>(it->second);

				if (type == XE_EVENT_TYPE_LOG) {
					XSEventLog* XAPI_eventLog = reinterpret_cast<XSEventLog*>(event);
					velo::event::Log log;
					log.channel = XAPI_eventLog->channel;
					log.msg = XAPI_eventLog->msg;
					auto msgLen = log.msg.length() - 1;
					if (log.msg[msgLen] == -1) {
						free((void*)XAPI_eventLog->msg);
						log.msg[msgLen] = 0x00;
					}
					log.severity = static_cast<velo::event::Log::Severity>(XAPI_eventLog->severity);
					_qBus->post(log);
					
					return 1;
				}
			}
			return XE_ERROR;
		}
		Xuint32 subscribeEvent(XHQEventBus qBus, Xconst Xuint64 type, Xconst Xvoid Xconstptr fn) {
			auto it = registries.find(qBus.unused);
			if (it != registries.end()) {
				velo::QEventBus* _qBus = reinterpret_cast<velo::QEventBus*>(it->second);

				switch (type) {
				default:
					break;

				case XE_EVENT_TYPE_CLIENT_CONNECT:
				{
					auto cb = reinterpret_cast<Xuint32(*)(Xconst Xvoid Xconstptr)>(fn);

					_qBus->subscribe<event::client::Connect>([cb](const event::client::Connect& e) {
						XSEventClientConnect ecc{};
						ecc.localPort = e.tcpClient->getLocalPort();
						ecc.remotePort = e.tcpClient->getRemotePort();
						std::string address = e.tcpClient->getRemoteAddress();
						ecc.tcpClient.unused = e.tcpClient->getNativeHandle();
						ecc.remoteAddress = address.c_str();
						cb((void*)&ecc);
						// clear the data after use!
						memset(&ecc, 0, sizeof(XSEventClientConnect));
						});
				}break;
				}
			}
			return 1;
		}
		Xvoid* memAlloc(Xuint64 size) {
			return malloc(size);
		}
		Xvoid memFree(Xvoid* mem) {
			free(mem);
		}
	} // ctx

	namespace tcp_client {
		Xuint64 getNativeHandle(XHTCPClient client) {
			return 0;
		}
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

	Xint32 createDeviceAndContext(XIDevice** device, XIContext** ctx) {
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

		const_cast<XIDeviceVTable*>(device_vtbl)->addRef = velo::device::addRef;
		const_cast<XIDeviceVTable*>(device_vtbl)->release = velo::device::release;
		const_cast<XIDeviceVTable*>(device_vtbl)->createQEventBus = velo::device::createQEventBus;

		const_cast<XIContextVTable*>(ctx_vtbl)->addRef = velo::ctx::addRef;
		const_cast<XIContextVTable*>(ctx_vtbl)->release = velo::ctx::release;
		const_cast<XIContextVTable*>(ctx_vtbl)->postEvent = velo::ctx::postEvent;
		const_cast<XIContextVTable*>(ctx_vtbl)->subscribeEvent = velo::ctx::subscribeEvent;
		const_cast<XIContextVTable*>(ctx_vtbl)->memAlloc = velo::ctx::memAlloc;
		const_cast<XIContextVTable*>(ctx_vtbl)->memFree = velo::ctx::memFree;

		deviceImpl->iface.vtbl = device_vtbl;
		ctxImpl->iface.vtbl = ctx_vtbl;

		*device = (XIDevice*)deviceImpl;
		*ctx = (XIContext*)ctxImpl;

		return 1;
	}

	Xint32 destroyDeviceAndContext(XIDevice** device, XIContext** ctx) {
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

} // velo