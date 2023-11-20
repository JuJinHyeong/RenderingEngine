#include "SceneProbe.h"
#include "SceneObject.h"
#include "imgui/imgui.h"

bool SceneProbe::PushNode(SceneObject& sceneObject) {
    const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
    const auto nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
        | (sceneObject.GetChildren().empty() ? ImGuiTreeNodeFlags_Leaf : 0)
        | (sceneObject.GetId() == selectedId ? ImGuiTreeNodeFlags_Selected : 0);
    const bool expanded = ImGui::TreeNodeEx((void*)&sceneObject, nodeFlags, sceneObject.GetName().c_str());
    if (ImGui::IsItemClicked()) {
		pSelectedNode = &sceneObject;
	}
    return expanded;
}

void SceneProbe::PopNode(SceneObject& sceneObject)
{
	ImGui::TreePop();
}

SceneObject* SceneProbe::GetSelectedNode() const noexcept
{
    return pSelectedNode;
}
