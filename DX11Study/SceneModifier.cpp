#include "SceneModifier.h"
#include "ApiService.h"

#include <iostream>

void SceneModifier::Modify(Scene& scene, const json& modifiedScene)
{

}

void SceneModifier::SendToGpt(Scene& scene, std::vector<std::string> makeables, const std::string message)
{
	json j;
	j["scene"] = scene.ToJson();
	j["content"] = message;
	std::string res = ApiService::httpPostRequest("http://127.0.0.1:1337/gpt/modify_scene", j);
	json modifiedScene = json::parse(res);
	std::cout << modifiedScene << std::endl;
	Modify(scene, modifiedScene);
}
