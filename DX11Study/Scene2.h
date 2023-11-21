#pragma once
#include <vector>
#include <string>

#include "JsonSerializable.h"
#include "SceneObject2.h"
#include "Mesh.h"
#include "Material.h"
#include "RenderGraph.h"

class Scene2 : public JsonSerializable {
public:
	Scene2(const std::string& name) noexcept;
	void AddSceneObject(std::shared_ptr<SceneObject2>& sceneObjectPtr) noexcept;
	void AddSceneObject(Graphics& gfx, const std::string& path) noexcept;

	void Submit(size_t channel) noexcept(!IS_DEBUG);
	void LinkTechnique(Rgph::RenderGraph rg);

	json ToJson() const override;
	const std::string& GetName() const noexcept;
	
private:
	std::string name;
	std::vector<std::shared_ptr<SceneObject2>> sceneObjectPtrs;
	std::vector<std::shared_ptr<Mesh>> meshPtrs;
	std::vector<std::shared_ptr<Material>> materialPtrs;
};