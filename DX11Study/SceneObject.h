#pragma once
#include <string>
#include <vector>
#include "Object.h"
#include "JsonSerializable.h"

namespace Rgph {
	class RenderGraph;
}

class SceneObject : public JsonSerializable {
public:
	enum Type {
		empty,
		light,
		object,
		camera
	};
public:
	SceneObject(const std::string& name);
	SceneObject(const std::string& name, std::unique_ptr<Object> pObject, const Type type) noexcept;
	json ToJson() const override;
	void AddChild(std::unique_ptr<SceneObject> child);
	void SetObject(std::unique_ptr<Object> object);
	const std::unique_ptr<Object>& GetObjectPtr() const noexcept;
	void Submit(size_t channel) noexcept(!IS_DEBUG);
	void LinkTechniques(Rgph::RenderGraph& rg);

	const Type GetType() const noexcept;
	const std::string& GetName() const noexcept;
private:
	unsigned int id = 0u;
	std::string name;
	Type type = Type::empty;
	std::unique_ptr<Object> pObject = nullptr;
	std::vector<std::unique_ptr<SceneObject>> children;
};