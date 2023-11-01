#include "Node.h"
#include "imgui/imgui.h"
#include "ModelProbe.h"
#include "Dump.h"

// Node
Node::Node(
	int id, 
	const std::string& name, 
	std::vector<Mesh*> meshPtrs, 
	std::unordered_map<std::string, unsigned int>* pNameToBoneIndexMap,
	std::vector<Bone>* pBoneMatrixes,
	const DirectX::XMMATRIX& transform,
	const DirectX::XMMATRIX& inverseRootTransform
) noexcept(!IS_DEBUG)
	:
	meshPtrs(std::move(meshPtrs)),
	name(name),
	id(id),
	pBoneMatrixes(pBoneMatrixes),
	pNameToBoneIndexMap(pNameToBoneIndexMap)
{
	DirectX::XMStoreFloat4x4(&this->transform, transform);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&this->inverseRootTransform, inverseRootTransform);
}

int Node::GetId() const noexcept {
	return id;
}

bool Node::HasChildren() const noexcept {
	return !childPtrs.empty();
}

// dfs
void Node::Accept(ModelProbe& probe) {
	if (probe.PushNode(*this)) {
		for (auto& cp : childPtrs) {
			cp->Accept(probe);
		}
		probe.PopNode(*this);
	}
}

void Node::Accept(TechniqueProbe& probe) {
	for (auto& mp : meshPtrs) {
		mp->Accept(probe);
	}
}

std::string Node::GetName() const noexcept {
	return name;
} 

aiVector3D InterpolatePosition(float tick, const std::vector<aiVectorKey>& positionKeys) {
	if (positionKeys.size() == 1) {
		return positionKeys[0].mValue;
	}

	unsigned int leftIndex = 0;
	for (unsigned int i = 0; i < positionKeys.size() - 1; i++) {
		float t = (float)positionKeys[i + 1].mTime;
		if (tick < t) {
			leftIndex = i;
			break;
		}
	}
	unsigned int rightIndex = leftIndex + 1;

	float deltaTime = positionKeys[rightIndex].mTime - positionKeys[leftIndex].mTime;
	float factor = (tick - (float)positionKeys[leftIndex].mTime) / deltaTime;

	const aiVector3D& start = positionKeys[leftIndex].mValue;
	const aiVector3D& end = positionKeys[rightIndex].mValue;
	return (1 - factor) * start + factor * end;
}

aiQuaternion InterpolateRotation(float tick, const std::vector<aiQuatKey>& rotationKeys) {
	aiQuaternion ret;
	if (rotationKeys.size() == 1) {
		return rotationKeys[0].mValue;
	}

	unsigned int leftIndex = 0;
	for (unsigned int i = 0; i < rotationKeys.size() - 1; i++) {
		float t = (float)rotationKeys[i + 1].mTime;
		if (tick < t) {
			leftIndex = i;
			break;
		}
	}
	unsigned int rightIndex = leftIndex + 1;

	float deltaTime = rotationKeys[rightIndex].mTime - rotationKeys[leftIndex].mTime;
	float factor = (tick - (float)rotationKeys[leftIndex].mTime) / deltaTime;

	const aiQuaternion& start = rotationKeys[leftIndex].mValue;
	const aiQuaternion& end = rotationKeys[rightIndex].mValue;
	aiQuaternion::Interpolate(ret, start, end, factor);
	return ret.Normalize();
}

aiVector3D InterpolateScaling(float tick, const std::vector<aiVectorKey>& scalingKeys) {
	if (scalingKeys.size() == 1) {
		return scalingKeys[0].mValue;
	}

	unsigned int leftIndex = 0;
	for (unsigned int i = 0; i < scalingKeys.size() - 1; i++) {
		float t = (float)scalingKeys[i + 1].mTime;
		if (tick < t) {
			leftIndex = i;
			break;
		}
	}
	unsigned int rightIndex = leftIndex + 1;

	float deltaTime = scalingKeys[rightIndex].mTime - scalingKeys[leftIndex].mTime;
	float factor = (tick - (float)scalingKeys[leftIndex].mTime) / deltaTime;

	const aiVector3D& start = scalingKeys[leftIndex].mValue;
	const aiVector3D& end = scalingKeys[rightIndex].mValue;
	return (1 - factor) * start + factor * end;
}
float prevTick = 0.0f;
void Node::Submit(size_t channel, DirectX::FXMMATRIX accumulatedTransform, const Animation& animation, float tick) const noexcept(!IS_DEBUG) {
	auto tf = DirectX::XMLoadFloat4x4(&transform);
	if (prevTick != tick) {
		Dump::Print("transform\n", Dump::MatrixToString(transform));
	}
	if (tick > 0.0f) {
		auto nameChannelMap = animation.GetNameChannelMap();
		auto it = nameChannelMap.find(name);
		if (it != nameChannelMap.end()) {
			auto& pNodeAnim = it->second;

			aiVector3D scaling = InterpolateScaling(tick, pNodeAnim.scalingKeys);
			DirectX::XMMATRIX scalingMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&scaling)));
			if (prevTick != tick) {
				Dump::Print("Scaling\n", Dump::MatrixToString(scalingMatrix));
			}

			aiQuaternion rotation = InterpolateRotation(tick, pNodeAnim.rotationKeys);
			DirectX::XMFLOAT4 rot = { rotation.x, rotation.y, rotation.z, rotation.w };
			DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rot));
			if (prevTick != tick) {
				Dump::Print("Rotation\n", Dump::MatrixToString(rotationMatrix));
			}

			aiVector3D position = InterpolatePosition(tick, pNodeAnim.positionKeys);
			DirectX::XMMATRIX positionMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&position)));

			if (prevTick != tick) {
				Dump::Print("Position\n", Dump::MatrixToString(positionMatrix));
			}

			tf = scalingMatrix * rotationMatrix * positionMatrix;
			if (prevTick != tick) {
				Dump::Print("calculation\n", Dump::MatrixToString(tf));
			}
		}
	}

	auto built = DirectX::XMLoadFloat4x4(&appliedTransform)
		* tf
		* accumulatedTransform;

	auto it = pNameToBoneIndexMap->find(name);
	if (it != pNameToBoneIndexMap->end()) {
		unsigned int boneIndex = pNameToBoneIndexMap->at(name);
		auto finalTransform = pBoneMatrixes->at(boneIndex).GetOffsetMatrixXM() * built * DirectX::XMLoadFloat4x4(&inverseRootTransform);
		pBoneMatrixes->at(boneIndex).SetFinalMatrix(finalTransform);
	}
	
	for (const auto pm : meshPtrs) {
		pm->Submit(channel, built);
	}
	for (const auto& pc : childPtrs) {
		pc->Submit(channel, built, animation, tick);
	}
}

const DirectX::XMFLOAT4X4& Node::GetAppliedTransform() const noexcept {
	return appliedTransform;
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG) {
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG) {
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}
