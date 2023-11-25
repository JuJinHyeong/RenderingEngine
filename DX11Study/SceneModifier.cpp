#include "SceneModifier.h"
#include "ApiService.h"

#include <iostream>

void SceneModifier::Modify(Graphics& gfx, Scene2& scene, const json& modifiedScene)
{
	scene.ModifyScene(gfx, modifiedScene);
}

void SceneModifier::SendToGpt(Graphics& gfx, Scene2& scene, std::vector<std::string>& makeables, const std::string message)
{
	json j;
	j["scene"] = scene.ToJson();
	j["content"] = message;
	j["makeables"] = makeables;
	std::string res = ApiService::httpPostRequest("http://127.0.0.1:1337/gpt/modify_scene", j);
	json modifiedScene = json::parse(res);
	Modify(gfx, scene, modifiedScene);
}
