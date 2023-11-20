#pragma once
#include "SceneObject.h"

class SceneProbe {
public:
	bool PushNode(SceneObject& sceneObject);
	void PopNode(SceneObject& sceneObject);
	SceneObject* GetSelectedNode() const noexcept;
private:
	SceneObject* pSelectedNode = nullptr;
};