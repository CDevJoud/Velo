#pragma once

#include <EASTL/string_view.h>
#include <EASTL/string.h>
#include <EASTL/unordered_map.h>
#include "typedefs.hpp"

namespace velo::net {
	class Http {
	public:
		Http();
		
		eastl::string_view getMethod() const;
		eastl::string_view getURL() const;
		eastl::string_view getVersion() const;
		eastl::string_view getBody() const;

		eastl::string getField(const eastl::string_view& name);

		void clear();

		void setMethod(const eastl::string_view& method);
		void setResponseCode(const i16 code, const eastl::string_view& msg);
		void setURL(const eastl::string_view& url);
		void setVersion(const eastl::string_view& version);
		void setField(const eastl::string_view& name, const eastl::string_view& value);
		void setBody(const eastl::string& body);

	private:
		friend class TCPClient;
		eastl::string method, url, version, body, respMsg;
		eastl::unordered_map<eastl::string, eastl::string> fields;
		i16 n16ResponseCode;
	};
}

