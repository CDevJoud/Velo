#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "PluginInterface.hpp"

namespace velo {
	PluginInterface::PluginInterface(XAPIDescriptor desc, Xvoid* hModule) : desc(desc), hModule(hModule) {

	}

	bool PluginInterface::instantiate() {
		XI_main = (XI_mainFn)GetProcAddress((HMODULE)hModule, "XI_main");
		XI_terminate = (XI_terminateFn)GetProcAddress((HMODULE)hModule, "XI_terminate");

		if (XI_main == nullptr || XI_terminate == nullptr) {
			return false; // bad plugin!
		}

		Xint32 res = XI_main(createDeviceAndContext);

		if (res == XE_ERROR) {
			// Fatal error!
			// terminating the plugin!
			res = XI_terminate(destroyDeviceAndContext);

			if (res == XE_ERROR) {
				// really bad plugin!
			}
			return false;
		}
		return true;
	}
}
