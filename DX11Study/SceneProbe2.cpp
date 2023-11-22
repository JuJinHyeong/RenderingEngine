#include "SceneProbe2.h"
#include "imgui/imgui.h"
#include "SceneObject2.h"

bool SceneProbe2::PushNode(SceneObject2* sceneObjectPtr)
{
    const int selectedId = (selectedNodePtr == nullptr) ? -1 : selectedNodePtr->GetId();
    const auto nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
        | (sceneObjectPtr->GetChildren().empty() ? ImGuiTreeNodeFlags_Leaf : 0)
        | (sceneObjectPtr->GetId() == selectedId ? ImGuiTreeNodeFlags_Selected : 0);
    const bool expanded = ImGui::TreeNodeEx((void*)(intptr_t)sceneObjectPtr->GetId(), nodeFlags, sceneObjectPtr->GetName().c_str());
    if (ImGui::IsItemClicked()) {
        selectedNodePtr = sceneObjectPtr;
    }
    return expanded;
}

void SceneProbe2::PopNode(SceneObject2* sceneObject)
{
    ImGui::TreePop();
}

SceneObject2* SceneProbe2::GetSelectedNodePtr() const noexcept
{
    return selectedNodePtr;
}
