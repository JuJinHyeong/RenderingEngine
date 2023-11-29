#include "SceneObject2.h"
#include "json.hpp"
#include "SceneProbe2.h"
#include "CustomMath.h"
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
	transform["position"] = { {"x", pos.x}, {"y", pos.y}, {"z", pos.z} };
	transform["scale"] = { {"x", scale.x}, {"y", scale.y}, {"z", scale.z} };
	transform["rotation"] = { {"x", quat.x}, {"y", quat.y}, {"z", quat.z}, {"w", quat.w} };
	j["transform"] = transform;

	if (!childPtrs.empty()) {
		j["children"] = json::array();
		for (const auto& childPtr : childPtrs) {
			j["children"].push_back(childPtr->ToJson());
		}
	}

	return j;
}

bool AreVectorsEqual(DirectX::XMVECTOR v1, DirectX::XMVECTOR v2, float epsilon = 1e-6f) {
	DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(v1, v2);
	return DirectX::XMVector4LessOrEqual(DirectX::XMVectorAbs(diff), DirectX::XMVectorReplicate(epsilon));
}
void SceneObject2::Modify(Graphics& gfx, const json& modifiedObject) noexcept {
	using namespace DirectX;
	name = modifiedObject["name"];
	type = modifiedObject["type"];
	
	auto tf = modifiedObject["transform"];
	XMVECTOR pos, quat, scale;
	XMMatrixDecompose(&scale, &quat, &pos, localTransform);
	XMVECTOR modifiedPos = XMVectorSet(tf["position"]["x"], tf["position"]["y"], tf["position"]["z"], 1.0f);
	XMVECTOR modifiedQuat = XMVectorSet(tf["rotation"]["x"], tf["rotation"]["y"], tf["rotation"]["z"], tf["rotation"]["w"]);
	XMVECTOR modifiedScale = XMVectorSet(tf["scale"]["x"], tf["scale"]["y"], tf["scale"]["z"], 0.0f);
	
	if (!AreVectorsEqual(pos, modifiedPos) || !AreVectorsEqual(quat, modifiedQuat) || !AreVectorsEqual(scale, modifiedScale)) {
		// modified values are calculated. do not add.
		SetLocalTransform(XMMatrixAffineTransformation(modifiedScale, XMVectorZero(), modifiedQuat, modifiedPos));
	}

	for (size_t i = 0; i < childPtrs.size(); i++) {
		childPtrs[i]->Modify(gfx, modifiedObject["children"][i]);
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

void SceneObject2::Accept(TechniqueProbe& probe) noexcept {
	for (auto& mp : meshPtrs) {
		mp->Accept(probe);
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

const bool SceneObject2::IsActived() const noexcept
{
	return actived;
}

void SceneObject2::SetActived(bool actived) noexcept
{
	this->actived = actived;
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
	return ++uid;
}
