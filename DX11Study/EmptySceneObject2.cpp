#include "EmptySceneObject2.h"
using json = nlohmann::json;
EmptySceneObject2::EmptySceneObject2(const std::string& name) noexcept 
	:
	SceneObject2(name, SceneObject2::Type::empty)
{}

void EmptySceneObject2::Submit(size_t channel) const noexcept(!IS_DEBUG) {
	for (const auto& child : childPtrs) {
		child->Submit(channel);
	}
}

json EmptySceneObject2::ToJson() const noexcept {
	return SceneObject2::ToJson();
}
