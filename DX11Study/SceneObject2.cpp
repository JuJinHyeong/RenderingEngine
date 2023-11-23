#include "SceneObject2.h"
#include "json.hpp"
#include "SceneProbe2.h"
using json = nlohmann::json;

SceneObject2::SceneObject2(const std::string& name, const Type type) noexcept
	:
	id(GenerateId()),
	type(type),
	name(name),
	localTransform(DirectX::XMMatrixIdentity()),
	childPtrs(),
	meshPtrs()
{}

void SceneObject2::AddChild(const std::shared_ptr<SceneObject2>& childPtr) noexcept {
	childPtrs.push_back(childPtr);
}

json SceneObject2::ToJson() const noexcept {
	json j;
	j["id"] = id;
	j["name"] = name;
	j["type"] = type;

	json transform;
	DirectX::XMVECTOR posV;
	DirectX::XMVECTOR quatV;
	DirectX::XMVECTOR scaleV;
	DirectX::XMMatrixDecompose(&scaleV, &quatV, &posV, localTransform);
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 quat;
	DirectX::XMStoreFloat3(&pos, posV);
	DirectX::XMStoreFloat3(&scale, scaleV);
	DirectX::XMStoreFloat4(&quat, quatV);
	transform["position"] = { pos.x, pos.y, pos.z };
	transform["scale"] = { scale.x, scale.y, scale.z };
	transform["rotation"] = { quat.x, quat.y, quat.z, quat.w };
	j["transform"] = transform;

	if (!childPtrs.empty()) {
		j["children"] = json::array();
		for (const auto& childPtr : childPtrs) {
			j["children"].push_back(childPtr->ToJson());
		}
	}

	return j;
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
