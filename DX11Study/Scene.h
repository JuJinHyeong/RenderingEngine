#pragma once
#include <string>
#include <vector>
#include "SceneObject.h"
#include "JsonSerializable.h"

class Scene : public JsonSerializable {
public:
	Scene(const std::string& name);
	json ToJson() const override;
	void AddObject(std::unique_ptr<SceneObject> object);
private:
	std::string name;
	std::vector<std::unique_ptr<SceneObject>> objects;
};