#include "Scene2.h"
#include "json.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
// TODO: change to SceneException
#include "ModelException.h"
#include "Mesh.h"
#include "Node2.h"
#include "RenderGraph.h"
#include "PointLight2.h"
#include "Camera.h"
#include "imgui/imgui.h"
#include "CustomUtils.h"
#include "CubeMesh.h"
#include "Geometry.h"
#include "EmptySceneObject2.h"
#include "TestModelProbe.h"

using json = nlohmann::json;

Scene2::Scene2(const std::string& name) noexcept 
	:
	name(name),
	sceneObjectPtrs(),
	meshPtrs(),
	materialPtrs()
{
	makeableObjects["Nano"] = "models/nano_textured/nanosuit.obj";
	makeableObjects["Goblin"] = "models/gobber/GoblinX.obj";
	makeableObjects["Sponza"] = "models/sponza/sponza.obj";
	makeableObjects["Tree"] = "models/Tree/Tree.obj";
	makeableObjects["Cube"] = "cube";
	makeableObjects["Empty"] = "empty";
}

void Scene2::AddSceneObject(std::shared_ptr<SceneObject2>&& sceneObjectPtr) noexcept {
	AddSceneObjectMeshes(sceneObjectPtr);
	sceneObjectPtrs.push_back(std::move(sceneObjectPtr));
}

void Scene2::AddSceneObject(Graphics& gfx, const std::string& path, float scale) noexcept {
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(path.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);
	if (pScene == nullptr) {
		return;
	}
	
	size_t materialOffset = materialPtrs.size();
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		materialPtrs.push_back(std::make_shared<Material>(gfx, *(pScene->mMaterials[i]), path));
	}

	std::vector<std::shared_ptr<Mesh>> tempMeshPtrs;
	for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
		const auto& mesh = *pScene->mMeshes[i];
		tempMeshPtrs.push_back(std::make_shared<Mesh>(gfx, materialPtrs[materialOffset + mesh.mMaterialIndex], mesh, scale));
	}

	sceneObjectPtrs.push_back(std::make_shared<Node2>(*pScene->mRootNode, tempMeshPtrs));

	meshPtrs.insert(meshPtrs.end(), std::make_move_iterator(tempMeshPtrs.begin()), std::make_move_iterator(tempMeshPtrs.end()));
}

void Scene2::SetCameraContainer(std::unique_ptr<CameraContainer> cameraContainerPtr) noexcept {
	this->cameraContainerPtr = std::move(cameraContainerPtr);
}

void Scene2::ModifyScene(Graphics& gfx, const json& modifiedScene) noexcept {
	SetIfChanged(name, (std::string)modifiedScene["name"]);
	for (size_t i = 0; i < modifiedScene["objects"].size(); i++) {
		auto modifiedSceneObject = modifiedScene["objects"][i];
		if (modifiedSceneObject["id"] == 0) {
			std::string makingSceneObjectName = modifiedSceneObject["name"];
			auto it = makeableObjects.find(makingSceneObjectName);
			if (it != makeableObjects.end()) {
				if (it->first == "Cube") {
					AddSceneObject(std::make_shared<Geometry<CubeMesh>>(gfx, "cube"));
				}
				else if (it->first == "Empty") {
					AddSceneObject(std::make_shared<EmptySceneObject2>("empty"));
				}
				else {
					AddSceneObject(gfx, it->second);
				}
			}
			else {
				continue;
			}

			auto makedObjectTransform = modifiedSceneObject["transform"];
			DirectX::XMVECTOR posV = DirectX::XMVectorSet(
				makedObjectTransform["position"]["x"],
				makedObjectTransform["position"]["y"],
				makedObjectTransform["position"]["z"], 1.0f
			);
			DirectX::XMVECTOR quatV = DirectX::XMVectorSet(
				makedObjectTransform["rotation"]["x"],
				makedObjectTransform["rotation"]["y"],
				makedObjectTransform["rotation"]["z"],
				makedObjectTransform["rotation"]["w"]
			);
			DirectX::XMVECTOR scaleV = DirectX::XMVectorSet(
				makedObjectTransform["scale"]["x"],
				makedObjectTransform["scale"]["y"],
				makedObjectTransform["scale"]["z"], 1.0f
			);
			sceneObjectPtrs.back()->SetLocalTransform(DirectX::XMMatrixAffineTransformation(scaleV, DirectX::XMVectorZero(), quatV, posV));
		}
		else {
			sceneObjectPtrs[i]->Modify(gfx, modifiedScene["objects"][i]);
		}
	}
}

void Scene2::Bind(Graphics& gfx) noexcept(!IS_DEBUG) {
	for (auto& objPtr : sceneObjectPtrs) {
		const auto& pointLight = std::dynamic_pointer_cast<PointLight2>(objPtr);
		if (pointLight != nullptr) {
			pointLight->Bind(gfx, GetActiveCamera().GetMatrix());
		}
	}
}

void Scene2::Submit(size_t channel) noexcept(!IS_DEBUG) {
	for (auto& objPtr : sceneObjectPtrs) {
		if (objPtr->IsActived()) {
			objPtr->Submit(channel);
		}
	}
	cameraContainerPtr->Submit(channel);
}

void Scene2::LinkTechnique(Rgph::RenderGraph& rg) {
	for (auto& meshPtr : meshPtrs) {
		meshPtr->LinkTechniques(rg);
	}
	cameraContainerPtr->LinkTechniques(rg);
}

Camera& Scene2::GetActiveCamera() const {
	return cameraContainerPtr->GetActiveCamera();
}

json Scene2::ToJson() const noexcept {
	json scene;
	scene["name"] = name;
	scene["objects"] = json::array();
	for (const auto& objPtr : sceneObjectPtrs) {
		scene["objects"].push_back(objPtr->ToJson());
	}
	return scene;
}

const std::string& Scene2::GetName() const noexcept {
	return name;
}

const std::vector<std::shared_ptr<SceneObject2>>& Scene2::GetSceneObjects() const noexcept
{
	return sceneObjectPtrs;
}

const std::unordered_map<std::string, std::string>& Scene2::GetMakeableObjects() const noexcept {
	return makeableObjects;
}

void Scene2::ShowWindow() noexcept
{
	ImGui::Begin(name.c_str());
	ImGui::Columns(2, nullptr, true);
	ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Scene Hierachy");
	for (auto& sceneObjectPtr : sceneObjectPtrs) {
		sceneObjectPtr->Accept(probe);
	}
	ImGui::NextColumn();
	auto* selectedNode = probe.GetSelectedNodePtr();
	if (selectedNode != nullptr) {
		ImGui::Text(selectedNode->GetName().c_str());
		bool isDirty = false;
		const auto dcheck = [&isDirty](bool dirty) {isDirty = isDirty || dirty; };

		bool isActive = selectedNode->IsActived();
		ImGui::Checkbox("Active", &isActive);
		selectedNode->SetActived(isActive);

		auto& transform = selectedNode->GetLocalTransform();
		DirectX::XMVECTOR posV, quatV, scaleV;
		DirectX::XMMatrixDecompose(&scaleV, &quatV, &posV, transform);
		DirectX::XMFLOAT3 pos, scale;
		DirectX::XMFLOAT4 quat;
		DirectX::XMStoreFloat3(&pos, posV);
		DirectX::XMStoreFloat3(&scale, scaleV);
		DirectX::XMStoreFloat4(&quat, quatV);
		dcheck(ImGui::SliderFloat("pos X", &pos.x, -30.0f, 30.0f, "%.1f"));
		dcheck(ImGui::SliderFloat("pos Y", &pos.y, -30.0f, 30.0f, "%.1f"));
		dcheck(ImGui::SliderFloat("pos Z", &pos.z, -30.0f, 30.0f, "%.1f"));

		dcheck(ImGui::SliderFloat("scale X", &scale.x, 0.1f, 5.0f, "%.1f"));
		dcheck(ImGui::SliderFloat("scale Y", &scale.y, 0.1f, 5.0f, "%.1f"));
		dcheck(ImGui::SliderFloat("scale Z", &scale.z, 0.1f, 5.0f, "%.1f"));

		float pitch = asinf(2 * (quat.w * quat.x - quat.y * quat.z));
		float yaw = atan2f(2 * (quat.w * quat.y + quat.z * quat.x), 1 - 2 * (quat.x * quat.x + quat.y * quat.y));
		float roll = atan2f(2 * (quat.w * quat.z + quat.x * quat.y), 1 - 2 * (quat.y * quat.y + quat.z * quat.z));
		dcheck(ImGui::SliderAngle("angle X", &pitch, -89.0f,89.0f));
		//dcheck(ImGui::SliderAngle("angle Y", &yaw, -89.0f, 89.0f));
		dcheck(ImGui::SliderAngle("angle Z", &roll, -89.0f, 89.0f));

		ImGui::Text("quat %f %f %f %f", quat.x, quat.y, quat.z, quat.w);
		if (isDirty) {
			auto modifiedQuat = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
			selectedNode->SetLocalTransform(
				DirectX::XMMatrixScaling(scale.x, scale.y, scale.z)
				* DirectX::XMMatrixRotationQuaternion(modifiedQuat)
				* DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z));
		}
		TP tProbe;
		selectedNode->Accept(tProbe);
	}
	ImGui::End();
}

void Scene2::AddSceneObjectMeshes(const std::shared_ptr<SceneObject2>& sceneObjectPtr) noexcept {
	meshPtrs.insert(meshPtrs.end(), sceneObjectPtr->GetMeshes().begin(), sceneObjectPtr->GetMeshes().end());
	for (auto& childPtr : sceneObjectPtr->GetChildren()) {
		AddSceneObjectMeshes(childPtr);
	}
}
