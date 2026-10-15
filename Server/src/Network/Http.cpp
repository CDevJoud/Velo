#include "Network/Http.hpp"

namespace velo::net {
	Http::Http() {

	}
	eastl::string_view Http::getMethod() const {
		return this->method;
	}
	eastl::string_view Http::getURL() const {
		return this->url;
	}
	eastl::string_view Http::getVersion() const {
		return this->version;
	}
	eastl::string_view Http::getBody() const {
		return this->body;
	}
	eastl::string Http::getField(const eastl::string_view& name) {
		if (this->fields.count(name.data())) {
			return this->fields.at(name.data());
		}
		else {
			return "";
		}
	}

	void Http::clear() {
		this->method.clear();
		this->url.clear();
		this->version.clear();
		this->body.clear();
		this->fields.clear();
	}

	void Http::setMethod(const eastl::string_view& method) {
		this->method = method;
	}

	void Http::setResponseCode(const i16 code, const eastl::string_view& msg) {
		this->n16ResponseCode = code; this->respMsg = msg;
	}

	void Http::setURL(const eastl::string_view& url) {
		this->url = url;
	}

	void Http::setVersion(const eastl::string_view& version) {
		this->version = version;
	}

	void Http::setField(const eastl::string_view& name, const eastl::string_view& value) {
		this->fields[name.data()] = value.data();
	}

	void Http::setBody(const eastl::string& body) {
		this->body = body;
	}


}
