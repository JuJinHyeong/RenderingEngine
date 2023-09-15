#include "Bindable.h"

namespace Bind {
	std::string Bindable::GetUID() const noexcept {
		assert("GetUID must be override" && false);
		return "";
	}
}
