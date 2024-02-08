#pragma once
#include <string>
#include <vector>
#include "Object.h"
#include "JsonSerializable.h"

namespace Rgph {
	class RenderGraph;
}
class SceneProbe;

/*
class SceneObject is a Object container.
SceneObject itself cannot do anything. All Rendering or other data is in Object.
*/
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
	json ToJson() const noexcept override;
	void AddChild(std::shared_ptr<SceneObject> child);
	const std::vector<std::shared_ptr<SceneObject>>& GetChildren();
	void SetObject(std::unique_ptr<Object> object);
	const std::unique_ptr<Object>& GetObjectPtr() const noexcept;
	void Submit(size_t channel) noexcept(!IS_DEBUG);
	void LinkTechniques(Rgph::RenderGraph& rg);
	void Accept(SceneProbe& probe);

	const unsigned int GetId() const noexcept;
	const Type GetType() const noexcept;
	const std::string& GetName() const noexcept;
private:
	static unsigned int GenerateId() noexcept;
private:
	const unsigned int id;
	std::string name;
	Type type = Type::empty;
	std::unique_ptr<Object> pObject = nullptr;
	std::vector<std::shared_ptr<SceneObject>> children;
};