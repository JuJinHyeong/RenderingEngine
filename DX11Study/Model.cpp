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
#include "Dump.h"
#include <sstream>

#ifndef NO_DEBUG
#define DumpClear(...) Dump::ClearFile("model.txt");
#define Dump(...) Dump::WriteToFile("model.txt", __VA_ARGS__);
#else
#define DumpClear(...)
#define Dump(...)
#endif

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

		DumpClear();
		Dump("Parsing ", pScene->mNumMeshes, " meshes\n");
		for (size_t i = 0; i < pScene->mNumMeshes; i++) {
			const auto& mesh = *pScene->mMeshes[i];
			Dump("\nMesh ", i, " '", mesh.mName.C_Str(), "': vertices ", mesh.mNumVertices, " indices ", mesh.mNumFaces * 3, " bones ", mesh.mNumBones, "\n\n");
			for (size_t j = 0; j < mesh.mNumBones; j++) {
				const auto& bone = *mesh.mBones[j];
				bonePtrs.emplace_back(std::make_shared<Bone>(i, bone));

				unsigned int boneIndex = FindBoneIndex(bone);
				Dump("#", boneIndex, "Bone ", bone.mName.C_Str(), ": num vertices affected by this bone: ", bone.mNumWeights, "\n");
			}
		}


		for (size_t i = 0; i < pScene->mNumMeshes; i++) {
			const auto& mesh = *pScene->mMeshes[i];
			meshPtrs.push_back(std::make_unique<Mesh>(gfx, materials[mesh.mMaterialIndex], mesh, i, bonePtrs, boneNameIndexMap, boneOffsetMatrixes, scale));
		}

		Dump("\nTotal Bones: ", boneOffsetMatrixes.size(), "\n");
		for (auto pair : boneNameIndexMap) {
			Dump("#", pair.second, ":", pair.first, "\n");
		}
		Dump("\n");
		for (size_t i = 0; i < boneOffsetMatrixes.size(); i++) {
			std::string matStr = Dump::MatrixToString(boneOffsetMatrixes[i]);
			Dump("#", i, "\n", matStr, "\n");
		}
	}

	Dump("*******************************************************\n");
	Dump("Parsing Nodes\n");
	auto rootTransform = DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&pScene->mRootNode->mTransformation));
	DirectX::XMVECTOR determinant = XMMatrixDeterminant(rootTransform);
	rootInverseTransform = XMMatrixTranspose(XMMatrixInverse(&determinant, rootTransform));

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

std::unique_ptr<Node> Model::ParseNode(int& curId, const aiNode& node, float scale, int space, DirectX::FXMMATRIX& parentTransform) {
	const auto transform = ScaleTranslation(DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	)), scale);
	const auto accumulatedTransform = transform * parentTransform;

	if (boneNameIndexMap.find(node.mName.C_Str()) != boneNameIndexMap.end()) {
		unsigned int boneIndex = boneNameIndexMap[node.mName.C_Str()];
		auto finalTransform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&boneMatrixes[boneIndex].offsetMatrix)) * accumulatedTransform * rootInverseTransform;
		DirectX::XMStoreFloat4x4(&boneMatrixes[boneIndex].finalMatrix, finalTransform);
	}

	std::string pad(space, ' ');
	{
		// TODO: Remove this
		Dump(pad, "Node name: '", node.mName.C_Str(), "' num children ", node.mNumChildren, " num meshes ", node.mNumMeshes, "\n");
		Dump(pad, "Node Transformation:\n");
		DirectX::XMFLOAT4X4 tf;
		DirectX::XMStoreFloat4x4(&tf, DirectX::XMMatrixTranspose(transform));
		Dump(Dump::MatrixToString(tf, space));
		Dump("\n");
	}

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++) {
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(curId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++) {
		Dump(pad + "    ", "--- ", i, " ---\n");
		pNode->AddChild(ParseNode(curId, *node.mChildren[i], scale, space + 4, accumulatedTransform));
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

unsigned int Model::FindBoneIndex(const aiBone& bone) noexcept(!IS_DEBUG) {
	unsigned int index = -1;
	if (boneNameIndexMap.find(bone.mName.C_Str()) == boneNameIndexMap.end()) {
		index = (unsigned int)boneOffsetMatrixes.size();
		boneNameIndexMap[bone.mName.C_Str()] = index;
		DirectX::XMFLOAT4X4 offsetMatrix = *reinterpret_cast<const DirectX::XMFLOAT4X4*>(&bone.mOffsetMatrix);
		boneOffsetMatrixes.push_back(offsetMatrix);
		boneMatrixes.push_back({ offsetMatrix });
	}
	else {
		index = boneNameIndexMap[bone.mName.C_Str()];
	}
	assert(index != -1);
	return index;
}