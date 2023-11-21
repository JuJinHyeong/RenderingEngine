#include "SceneObject2.h"
#include "json.hpp"
using json = nlohmann::json;

SceneObject2::SceneObject2(const std::string& name) noexcept 
	:
	id(GenerateId()),
	type(Type::empty),
	name(name),
	localTransform(DirectX::XMMatrixIdentity()),
	childPtrs(),
	meshPtrs()
{}

void SceneObject2::AddChild(std::shared_ptr<SceneObject2>& childPtr) noexcept {
	childPtrs.push_back(std::move(childPtr));
}

void SceneObject2::Submit(size_t channel, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG) {
	auto built = accumulatedTransform * localTransform;
	for (const auto& pm : meshPtrs) {
		pm->Submit(channel, built);
	}
	for (const auto& pc : childPtrs) {
		pc->Submit(channel, built);
	}
}

json SceneObject2::ToJson() const {
	json so;
	so["id"] = id;
	so["name"] = name;
	return so;
}

const int SceneObject2::GetId() const noexcept {
	return id;
}

const std::string& SceneObject2::GetName() const noexcept {
	return name;
}

const DirectX::XMMATRIX& SceneObject2::GetLocalTransform() const noexcept {
	return localTransform;
}

void SceneObject2::SetLocalTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG) {
	localTransform = transform;
}

bool SceneObject2::HasChildren() const noexcept {
	return !childPtrs.empty();
}

int SceneObject2::GenerateId() noexcept {
	static int uid = 0;
	return uid++;
}
