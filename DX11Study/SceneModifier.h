#pragma once
#include <vector>
#include <string>

#include "Scene.h"
#include "json.hpp"

class SceneModifier {
	using json = nlohmann::json;
public:
	static void Modify(Scene& scene, const json& modifiedScene);
	static void SendToGpt(Scene& scene, std::vector<std::string> makeables, const std::string message);
};