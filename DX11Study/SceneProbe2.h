#pragma once
#include "TechniqueProbe.h"
#include "DynamicConstant.h"
#include "imgui/imgui.h"

class SceneObject2;

class SceneProbe2 {
public:
	bool PushNode(SceneObject2* sceneObjectPtr);
	void PopNode(SceneObject2* sceneObjectPtr);
	SceneObject2* GetSelectedNodePtr() const noexcept;
private:
	SceneObject2* selectedNodePtr = nullptr;
};