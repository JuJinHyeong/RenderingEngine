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
using json = nlohmann::json;

Scene2::Scene2(const std::string& name) noexcept 
	:
	name(name),
	sceneObjectPtrs(),
	meshPtrs(),
	materialPtrs()
{}

void Scene2::AddSceneObject(std::shared_ptr<SceneObject2>& sceneObjectPtr) noexcept {
	sceneObjectPtrs.push_back(std::move(sceneObjectPtr));
}

void Scene2::AddSceneObject(Graphics& gfx, const std::string& path) noexcept {
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
		tempMeshPtrs.push_back(std::make_shared<Mesh>(gfx, materialPtrs[materialOffset + mesh.mMaterialIndex], mesh));
	}

	sceneObjectPtrs.push_back(std::make_shared<Node2>(*pScene->mRootNode, tempMeshPtrs));

	meshPtrs.insert(meshPtrs.end(), std::make_move_iterator(tempMeshPtrs.begin()), std::make_move_iterator(tempMeshPtrs.end()));
}

void Scene2::Submit(size_t channel) noexcept(!IS_DEBUG) {
	for (auto& objPtr : sceneObjectPtrs) {
		objPtr->Submit(channel, DirectX::XMMatrixIdentity());
	}
}

void Scene2::LinkTechnique(Rgph::RenderGraph& rg) {
	for (auto& meshPtr : meshPtrs) {
		meshPtr->LinkTechniques(rg);
	}
}

json Scene2::ToJson() const {
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