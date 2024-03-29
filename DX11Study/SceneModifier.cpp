#include "SceneModifier.h"
#include "ApiService.h"

#include <iostream>

void SceneModifier::Modify(Graphics& gfx, Scene2& scene, const json& modifiedScene)
{
	try {
		scene.ModifyScene(gfx, modifiedScene);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

bool SceneModifier::SendToGpt(Graphics& gfx, Scene2& scene, std::vector<std::string>& makeables, const std::string message)
{
	try {
		json j;
		j["scene"] = scene.ToJson();
		j["content"] = message;
		j["makeables"] = makeables;
		std::string res = ApiService::httpPostRequest("http://127.0.0.1:1337/gpt/modify_scene", j);
		json modifiedScene = json::parse(res);
		Modify(gfx, scene, modifiedScene);
		return true;
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return false;
	}
}
