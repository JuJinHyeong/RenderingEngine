#pragma once
#include <string>
#include <vector>
#include "SceneObject.h"
#include "JsonSerializable.h"

class Camera;
class Graphics;

class Scene : public JsonSerializable {
public:
	Scene(const std::string& name);
	json ToJson() const override;
	void AddObject(std::shared_ptr<SceneObject> object);
	void Submit(size_t channel) noexcept(!IS_DEBUG);
	const std::vector<std::shared_ptr<SceneObject>>& GetObjects() const noexcept;
	Camera& GetActiveCamera();
	void Bind(Graphics& gfx) noexcept(!IS_DEBUG);
	void ShowWindow();
private:
	std::string name;
	std::vector<std::shared_ptr<SceneObject>> pSceneObjects;
};