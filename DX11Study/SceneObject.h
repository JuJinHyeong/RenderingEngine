#pragma once
#include <string>
#include <vector>
#include "Object.h"
#include "JsonSerializable.h"

class SceneObject : public JsonSerializable {
public:
	SceneObject(const std::string& name);
	json ToJson() const override;
	void AddChild(std::unique_ptr<SceneObject> child);
	void SetObject(std::unique_ptr<Object> object);
private:
	unsigned int id = 0u;
	std::string name;
	std::unique_ptr<Object> object;
	std::vector<std::unique_ptr<SceneObject>> children;
};