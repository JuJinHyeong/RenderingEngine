#include "Model.h"
#include "imgui/imgui.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "ModelException.h"
#include "Material.h"

// Model

// ModelWindow class doesn't need to be read from another header.
// so ModelWindow class is defined in cpp file.
class ModelWindow {
public:
	void Show(const char* windowName, const Node& root) noexcept(!IS_DEBUG) {
		windowName = windowName ? windowName : "Model";
		if (ImGui::Begin(windowName)) {
			ImGui::Columns(2, nullptr, true);
			root.ShowTree(pSelectedNode);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr) {
				ImGui::Text("Id: %d", pSelectedNode->GetId());
				auto& transform = transforms[pSelectedNode->GetId()];
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);

				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
			}
		}
		ImGui::End();
	}
	DirectX::XMMATRIX GetTransform() const noexcept(!IS_DEBUG) {
		assert(pSelectedNode != nullptr);
		const auto& transform = transforms.at(pSelectedNode->GetId());
		return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw)
			* DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}
	Node* GetSelectedNode() const noexcept(!IS_DEBUG) {
		return pSelectedNode;
	}
private:
	Node* pSelectedNode = nullptr;
	struct TransformParameters {
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> transforms;
};

Model::~Model() noexcept(!IS_DEBUG) {}

Model::Model(Graphics& gfx, const std::string& pathStr, const float scale)
	:
	pWindow(std::make_unique<ModelWindow>()) {
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(pathStr.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if (pScene == nullptr) {
		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
	}

	std::vector<Material> materials;
	materials.reserve(pScene->mNumMaterials);
	for (size_t i = 0; i < pScene->mNumMaterials; i++) {
		materials.emplace_back(gfx, *(pScene->mMaterials[i]), pathStr);
	}

	for (size_t i = 0; i < pScene->mNumMeshes; i++) {
		const auto& mesh = *pScene->mMeshes[i];
		meshPtrs.push_back(std::make_unique<Mesh>(gfx, materials[mesh.mMaterialIndex], mesh, scale));
	}

	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode);
}

void Model::ShowWindow(const char* windowName) noexcept(!IS_DEBUG) {
	pWindow->Show(windowName, *pRoot);
}

void Model::Submit(FrameCommander& frame) const noexcept(!IS_DEBUG) {
	//pWindow->ApplyParameters();
	pRoot->Submit(frame, DirectX::XMMatrixIdentity());
}

std::unique_ptr<Node> Model::ParseNode(int& curId, const aiNode& node) {
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++) {
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(curId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++) {
		pNode->AddChild(ParseNode(curId, *node.mChildren[i]));
	}

	return pNode;
}

void Model::SetRootTransform(DirectX::FXMMATRIX tf) noexcept {
	pRoot->SetAppliedTransform(tf);
}
