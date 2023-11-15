#include "Scene.h"
#include "json.hpp"
#include "CameraContainer.h"
#include "PointLight.h"
#include "Camera.h"
#include "imgui/imgui.h"
#include "ApiService.h"

#include <memory>
#include <iostream>

using json = nlohmann::json;
Scene::Scene(const std::string& name)
    :
    name(name),
    pSceneObjects()
{
}
json Scene::ToJson() const
{
    json j;
    j["name"] = name;
    j["objects"] = json::array();
    for(auto&& object : pSceneObjects){
        j["objects"].push_back(object->ToJson());
    }
    return j;
}

void Scene::AddObject(std::shared_ptr<SceneObject> object)
{
    // TODO: check objects like cameras...
    pSceneObjects.push_back(std::move(object));
}

void Scene::Submit(size_t channel) noexcept(!IS_DEBUG)
{
    for (auto& object : pSceneObjects) {
		object->Submit(channel);
	}
}

const std::vector<std::shared_ptr<SceneObject>>& Scene::GetObjects() const noexcept
{
    return pSceneObjects;
}

Camera& Scene::GetActiveCamera()
{
    for (const auto& pSceneObject : pSceneObjects) {
        if (pSceneObject->GetType() == SceneObject::Type::camera) {
            const auto& objPtr = pSceneObject->GetObjectPtr();
            return dynamic_cast<CameraContainer&>(*objPtr).GetActiveCamera();
        }
    }
    assert("Failed to find camera!" && false);
}

void Scene::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
{
    for (const auto& pSceneObject : pSceneObjects) {
        if (pSceneObject->GetType() == SceneObject::Type::light) {
            const auto& objPtr = pSceneObject->GetObjectPtr();
            Camera& cam = GetActiveCamera();
            dynamic_cast<PointLight&>(*objPtr).Bind(gfx, cam.GetMatrix());
        }
    }
}

char buffer[256] = "";
void Scene::ShowWindow()
{
    ImGui::Begin(name.c_str());
    ImGui::TextColored({ 1.0f, 1.0f, 0.2f, 0.0 }, "Object Name List");
    for (const auto& pSceneObject : pSceneObjects) {
        ImGui::Text(pSceneObject->GetName().c_str());
    }
    ImGui::InputText("gpt", buffer, IM_ARRAYSIZE(buffer));
    json j;
    j["scene"] = ToJson();
    j["content"] = buffer;
    if (ImGui::Button("GPT")) {
        std::string res = ApiService::httpPostRequest("http://127.0.0.1:1337/gpt/modify_scene", j);
        std::cout << res << std::endl;
    }
    ImGui::End();
}
