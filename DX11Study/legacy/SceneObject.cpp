#include "SceneObject.h"
#include "ExtendedXMMath.h"
#include "imgui/imgui.h"
#include "SceneProbe.h"

using json = nlohmann::json;
SceneObject::SceneObject(const std::string& name)
    :
    id(GenerateId()),
    name(name)
{
}
SceneObject::SceneObject(const std::string& name, std::unique_ptr<Object> pObject, const Type type) noexcept
    :
    id(GenerateId()),
    name(name),
    pObject(std::move(pObject)),
    type(type)
{
}
json SceneObject::ToJson() const noexcept
{
	json j{
		{"id", id},
		{"name", name},
        {"position", {
            {"x", 0}, {"y", 0}, {"z", 0}
        }},
		{"quaternion", {
			{"x", 0}, {"y", 0}, {"z", 0}, {"w", 0}
		}},
		{"scale", {
			{"x", 1.0f}, {"y", 1.0f}, {"z", 1.0f}
		}}
	};
    return j;
}

void SceneObject::AddChild(std::shared_ptr<SceneObject> child)
{
    children.push_back(std::move(child));
}

const std::vector<std::shared_ptr<SceneObject>>& SceneObject::GetChildren()
{
    return children;
}

void SceneObject::SetObject(std::unique_ptr<Object> object)
{
    pObject = std::move(object);
}

const std::unique_ptr<Object>& SceneObject::GetObjectPtr() const noexcept
{
    return pObject;
}

void SceneObject::Submit(size_t channel) noexcept(!IS_DEBUG)
{
    if (type != Type::empty) {
        pObject->Submit(channel);
    }
    for (auto& child : children) {
        child->Submit(channel);
    }
}

void SceneObject::LinkTechniques(Rgph::RenderGraph& rg) {
    if (type != Type::empty) {
        pObject->LinkTechniques(rg);
    }
    for (auto& child : children) {
        child->LinkTechniques(rg);
    }
}

void SceneObject::Accept(SceneProbe& probe)
{
    if (probe.PushNode(*this)) {
        for (auto& child : children) {
			child->Accept(probe);
		}
        probe.PopNode(*this);
    }
}

const unsigned int SceneObject::GetId() const noexcept
{
    return id;
}

const SceneObject::Type SceneObject::GetType() const noexcept
{
    return type;
}

const std::string& SceneObject::GetName() const noexcept
{
    return name;
}

unsigned int SceneObject::GenerateId() noexcept
{
    static unsigned int unique_id = 0;
    return unique_id++;
}
