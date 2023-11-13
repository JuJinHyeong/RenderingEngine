#include "Scene.h"
#include "json.hpp"

using json = nlohmann::json;
Scene::Scene(const std::string& name)
    :
    name(name)
{
}
json Scene::ToJson() const
{
    json j;
    j["name"] = name;
    j["objects"] = json::array();
    for(auto&& object : objects){
        j["objects"].push_back(object->ToJson());
    }
    return j;
}

void Scene::AddObject(std::unique_ptr<SceneObject> object)
{
    objects.push_back(std::move(object));
}
