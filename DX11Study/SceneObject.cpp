#include "SceneObject.h"
#include "ExtendedXMMath.h"

using json = nlohmann::json;
SceneObject::SceneObject(const std::string& name)
    :
    name(name)
{
}
json SceneObject::ToJson() const
{
    json j;
    j["id"] = id;
    j["name"] = name;
    const auto objTransform = object->GetTransform();
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
    this->object = std::move(object);
}
