#include "SceneObject.h"
#include "ExtendedXMMath.h"

using json = nlohmann::json;
SceneObject::SceneObject(const std::string& name)
    :
    name(name)
{
}
SceneObject::SceneObject(const std::string& name, std::unique_ptr<Object> pObject, const Type type) noexcept
    :
    name(name),
    pObject(std::move(pObject)),
    type(type)
{
}
json SceneObject::ToJson() const
{
    json j;
    j["id"] = id;
    j["name"] = name;
    const auto& objTransform = pObject->GetTransform();
    auto tf = DirectX::XMLoadFloat4x4(&objTransform);
    auto pos = GetPositionFromMatrix(tf);
    auto rot = GetRotationFromMatrix(tf);
    auto scale = GetScaleFromMatrix(tf);
    json transform;
    transform["position"] = json::array({pos.x, pos.y, pos.z});
    transform["rotation"] = json::array({rot.x, rot.y, rot.z, rot.w});
    transform["scale"] = json::array({scale.x, scale.y, scale.z});
    j["transform"] = transform;
    return j;
}

void SceneObject::AddChild(std::unique_ptr<SceneObject> child)
{
    children.push_back(std::move(child));
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
    pObject->Submit(channel);
}

void SceneObject::LinkTechniques(Rgph::RenderGraph& rg)
{
    pObject->LinkTechniques(rg);
}

const SceneObject::Type SceneObject::GetType() const noexcept
{
    return type;
}

const std::string& SceneObject::GetName() const noexcept
{
    return name;
}
