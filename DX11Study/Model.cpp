#include "Model.h"
#include "imgui/imgui.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "ModelException.h"
#include "Material.h"
#include "ExtendedXMMath.h"
#include "ModelProbe.h"
#include "Bone.h"

// Model
Model::~Model() noexcept(!IS_DEBUG) {}

Model::Model(Graphics& gfx, const std::string& pathStr, const float scale, const bool test)
{
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

	if (test) {
		std::vector<Material> materials;
		materials.reserve(pScene->mNumMaterials);
		for (size_t i = 0; i < pScene->mNumMaterials; i++) {
			materials.emplace_back(gfx, *(pScene->mMaterials[i]), pathStr);
		}

		for (size_t i = 0; i < pScene->mNumMeshes; i++) {
			const auto& mesh = *pScene->mMeshes[i];
			meshPtrs.push_back(std::make_unique<Mesh>(gfx, materials[mesh.mMaterialIndex], mesh, scale));
		}
	}
	else {
		std::vector<Material2> materials;
		materials.reserve(pScene->mNumMaterials);
		for (size_t i = 0; i < pScene->mNumMaterials; i++) {
			materials.emplace_back(gfx, *(pScene->mMaterials[i]), pathStr);
		}


		for (size_t i = 0; i < pScene->mNumMeshes; i++) {
			const auto& mesh = *pScene->mMeshes[i];
			for (size_t j = 0; j < mesh.mNumBones; j++) {
				const auto& bone = *mesh.mBones[j];
				bonePtrs.emplace_back(std::make_shared<Bone>(mesh.mName.C_Str(), bone));
				if (boneNameIndexMap.find(bone.mName.C_Str()) == boneNameIndexMap.end()) {
					boneNameIndexMap[bone.mName.C_Str()] = boneOffsetMatrixes.size();
					boneOffsetMatrixes.push_back(bonePtrs.back()->GetOffsetMatrix());
				}
			}
		}


		for (size_t i = 0; i < pScene->mNumMeshes; i++) {
			const auto& mesh = *pScene->mMeshes[i];
			meshPtrs.push_back(std::make_unique<Mesh>(gfx, materials[mesh.mMaterialIndex], mesh, bonePtrs, boneNameIndexMap, boneOffsetMatrixes, scale));
		}
	}

	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode, scale);
}

//void Model::ShowWindow(const char* windowName) noexcept(!IS_DEBUG) {
//	pWindow->Show(windowName, *pRoot);
//}

void Model::Submit(size_t channel) const noexcept(!IS_DEBUG) {
	//pWindow->ApplyParameters();
	pRoot->Submit(channel, DirectX::XMMatrixIdentity());
}

void Model::Accept(ModelProbe& probe) {
	pRoot->Accept(probe);
}

std::unique_ptr<Node> Model::ParseNode(int& curId, const aiNode& node, float scale) {
	const auto transform = ScaleTranslation(DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	)), scale);

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++) {
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(curId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++) {
		pNode->AddChild(ParseNode(curId, *node.mChildren[i], scale));
	}

	return pNode;
}

void Model::SetRootTransform(DirectX::FXMMATRIX tf) noexcept {
	pRoot->SetAppliedTransform(tf);
}

void Model::LinkTechniques(Rgph::RenderGraph& rg) {
	for (auto& pMesh : meshPtrs) {
		pMesh->LinkTechniques(rg);
	}
}
