#pragma once
#include <vector>
#include <string>

#include "json.hpp"
#include "Scene2.h"


class SceneModifier {
	using json = nlohmann::json;
public:
	static void Modify(Graphics& gfx, Scene2& scene, const json& modifiedScene);
	static void SendToGpt(Graphics& gfx, Scene2& scene, std::vector<std::string>& makeables, const std::string message);
};