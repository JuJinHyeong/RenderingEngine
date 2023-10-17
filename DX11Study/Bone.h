#pragma once
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <DirectXMath.h>
#include <string>
#include <vector>

class Bone {
public:
	Bone(const aiBone& bone);
private:
	std::string name;
	DirectX::XMFLOAT4X4 offset;
	std::vector<std::pair<unsigned int, float>> weights;
};