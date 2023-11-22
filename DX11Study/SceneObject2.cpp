#include "SceneObject2.h"
#include "json.hpp"
#include "SceneProbe2.h"
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

void SceneObject2::Submit(size_t channel) const noexcept(!IS_DEBUG) {
	for (const auto& pm : meshPtrs) {
		pm->Submit(channel);
	}
	for (const auto& pc : childPtrs) {
		pc->Submit(channel);
	}
}

void SceneObject2::Accept(SceneProbe2& probe) noexcept
{
	if (probe.PushNode(this)) {
		for (auto& child : childPtrs) {
			child->Accept(probe);
		}
		probe.PopNode(this);
	}
}

const int SceneObject2::GetId() const noexcept {
	return id;
}

const SceneObject2::Type SceneObject2::GetType() const noexcept {
	return type;
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

const std::vector<std::shared_ptr<SceneObject2>>& SceneObject2::GetChildren() const noexcept {
	return childPtrs;
}

const std::vector<std::shared_ptr<Drawable>>& SceneObject2::GetMeshes() const noexcept {
	return meshPtrs;
}

bool SceneObject2::HasChildren() const noexcept {
	return !childPtrs.empty();
}

int SceneObject2::GenerateId() noexcept {
	static int uid = 0;
	return uid++;
}
