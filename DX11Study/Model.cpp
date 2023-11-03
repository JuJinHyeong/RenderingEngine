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

		for (size_t meshIdx = 0; meshIdx < pScene->mNumMeshes; meshIdx++) {
			const auto& mesh = *pScene->mMeshes[meshIdx];
			Dump("\nMesh ", meshIdx, " '", mesh.mName.C_Str(), "': vertices ", mesh.mNumVertices, " indices ", mesh.mNumFaces * 3, " bones ", mesh.mNumBones, "\n\n");
			for (size_t j = 0; j < mesh.mNumBones; j++) {
				const auto& bone = *mesh.mBones[j];
				unsigned int boneIndex = ResolveBoneIndex(bone);
				Dump("#", boneIndex, "Bone ", bone.mName.C_Str(), ": num vertices affected by this bone: ", bone.mNumWeights, "\n");
			}
		}

		for (size_t i = 0; i < pScene->mNumMeshes; i++) {
			const auto& mesh = *pScene->mMeshes[i];
			meshPtrs.push_back(std::make_unique<Mesh>(gfx, materials[mesh.mMaterialIndex], mesh, nameBoneIndexMap, &bones, scale));
		}
	}

	for (int i = 0; i < pScene->mNumAnimations; i++) {
		animations.emplace_back(*pScene->mAnimations[i]);
	}

	Dump("*******************************************************\n");
	Dump("Parsing Nodes\n");
	auto rootTransform = DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&pScene->mRootNode->mTransformation));
	DirectX::XMVECTOR determinant = XMMatrixDeterminant(rootTransform);
	rootInverseTransform = XMMatrixTranspose(XMMatrixInverse(&determinant, rootTransform));

	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode, scale);
	
	for (auto pair : nameBoneIndexMap) {
		unsigned int boneIndex = pair.second;
		Dump("#", boneIndex, ":", pair.first, "\n");
		Dump("Offset Matrix\n", Dump::MatrixToString(bones[boneIndex].GetOffsetMatrix()));
		Dump("Final Matrix\n", Dump::MatrixToString(bones[boneIndex].GetFinalMatrix()), "\n");
	}
}

void Model::Submit(size_t channel) const noexcept(!IS_DEBUG) {
	pRoot->Submit(channel, DirectX::XMMatrixIdentity(), animations[0], animationTick);
}

void ShowMatrix(DirectX::XMFLOAT4X4& mat) {
	
}

void Model::Accept(ModelProbe& probe) {
	pRoot->Accept(probe);
}

std::unique_ptr<Node> Model::ParseNode(int& curId, const aiNode& node, float scale, int space) {
	const auto transform = ScaleTranslation(DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	)), scale);


	std::string pad(space, ' ');
	{
		// TODO: Remove this
		Dump(pad, "Node name: '", node.mName.C_Str(), "' num children ", node.mNumChildren, " num meshes ", node.mNumMeshes, "\n");
		Dump(pad, "Node Transformation:\n");
		Dump(Dump::MatrixToString(DirectX::XMMatrixTranspose(transform), space));
		Dump("\n");
	}

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++) {
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(curId++, node.mName.C_Str(), std::move(curMeshPtrs), &nameBoneIndexMap, &bones, transform, rootInverseTransform);
	for (size_t i = 0; i < node.mNumChildren; i++) {
		Dump(pad + "    ", "--- ", i, " ---\n");
		pNode->AddChild(ParseNode(curId, *node.mChildren[i], scale, space + 4));
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

const std::vector<Bone>& Model::GetBoneMatrixes() const noexcept
{
	return bones;
}

float& Model::GetAnimationTick() noexcept
{
	return animationTick;
}

unsigned int Model::ResolveBoneIndex(const aiBone& bone) noexcept(!IS_DEBUG) {
	auto it = nameBoneIndexMap.find(bone.mName.C_Str());
	unsigned int index = -1;
	if (it == nameBoneIndexMap.end()) {
		index = (unsigned int)bones.size();
		nameBoneIndexMap[bone.mName.C_Str()] = index;
		DirectX::XMFLOAT4X4 offsetMatrix = *reinterpret_cast<const DirectX::XMFLOAT4X4*>(&bone.mOffsetMatrix);
		bones.emplace_back(bone);
	}
	else {
		index = it->second;
	}
	assert(index != -1);
	return index;
}