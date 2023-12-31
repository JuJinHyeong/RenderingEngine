#pragma once
#include <vector>
#include <string>

#include "JsonSerializable.h"
#include "SceneObject2.h"
#include "Mesh.h"
#include "Material.h"
#include "RenderGraph.h"
#include "CameraContainer.h"
#include "SceneProbe2.h"
#include <unordered_map>

class Scene2 : public JsonSerializable {
public:
	Scene2(const std::string& name) noexcept;
	void AddSceneObject(std::shared_ptr<SceneObject2>&& sceneObjectPtr) noexcept;
	void AddSceneObject(Graphics& gfx, const std::string& path, float scale = 1.0f) noexcept;
	void SetCameraContainer(std::unique_ptr<CameraContainer> cameraContainerPtr) noexcept;
	void ModifyScene(Graphics& gfx, const json& modifiedScene);

	void Bind(Graphics& gfx) noexcept(!IS_DEBUG);
	void Submit(size_t channel) noexcept(!IS_DEBUG);
	void LinkTechnique(Rgph::RenderGraph& rg);
	Camera& GetActiveCamera() const;

	json ToJson() const noexcept override;
	const std::string& GetName() const noexcept;
	const std::vector<std::shared_ptr<SceneObject2>>& GetSceneObjects() const noexcept;
	const std::unordered_map<std::string, std::string>& GetMakeableObjects() const noexcept;
	void ShowWindow() noexcept;

private:
	void AddSceneObjectMeshes(const std::shared_ptr<SceneObject2>& sceneObjectPtr) noexcept;
	
private:
	std::string name;
	std::vector<std::shared_ptr<SceneObject2>> sceneObjectPtrs;
	std::vector<std::shared_ptr<Drawable>> meshPtrs;
	std::vector<std::shared_ptr<Material>> materialPtrs;
	std::unique_ptr<CameraContainer> cameraContainerPtr;

	std::unordered_map<std::string, std::string> makeableObjects;

	SceneProbe2 probe;
};