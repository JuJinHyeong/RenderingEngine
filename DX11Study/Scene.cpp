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

//char buffer[256] = "";
void Scene::ShowWindow()
{
    ImGui::Begin(name.c_str());
    ImGui::Columns(2, nullptr, true);
    ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Scene Hierachy");
    for (const auto& pSceneObject : pSceneObjects) {
        pSceneObject->Accept(sceneProbe);
    }
    ImGui::NextColumn();
    auto pSelectedNode = sceneProbe.GetSelectedNode();
    if (pSelectedNode != nullptr) {
        ImGui::Text(pSelectedNode->GetName().c_str());
        auto& pObject = pSelectedNode->GetObjectPtr();
        //auto& objectPos = pObject->GetPos();
        //ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Translation");
        //ImGui::SliderFloat("X", &objectPos.x, -10.0f, 10.0f);
        //ImGui::SliderFloat("Y", &objectPos.y, -10.0f, 10.0f);
        //ImGui::SliderFloat("Z", &objectPos.z, -10.0f, 10.0f);
        //ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
        //dcheck(ImGui::SliderFloat("X-rotation", &tf.xRot, -PI, PI));
        //dcheck(ImGui::SliderFloat("Y-rotation", &tf.yRot, -PI, PI));
        //dcheck(ImGui::SliderFloat("Z-rotation", &tf.zRot, -PI, PI));
        //auto diff = dx::XMMatrixTranspose(dx::XMMatrixRotationX(tf.xRot) *
        //    dx::XMMatrixRotationY(tf.yRot) *
        //    dx::XMMatrixRotationZ(tf.zRot) *
        //    dx::XMMatrixTranslation(tf.x, tf.y, tf.z));

    }
    ImGui::End();
}

const std::string& Scene::GetName() const noexcept
{
    return name;
}
