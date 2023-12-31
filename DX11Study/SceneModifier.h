#pragma once
#include <vector>
#include <string>

#include "Scene.h"
#include "json.hpp"
#include "Scene2.h"


class SceneModifier {
	using json = nlohmann::json;
public:
	static void Modify(Graphics& gfx, Scene2& scene, const json& modifiedScene);
	static bool SendToGpt(Graphics& gfx, Scene2& scene, std::vector<std::string>& makeables, const std::string message);
};