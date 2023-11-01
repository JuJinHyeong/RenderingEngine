#pragma once
#include <assimp/scene.h>
#include <unordered_map>
#include <span>

class Animation {
public:
	struct Channel {
		std::vector<aiVectorKey> positionKeys;
		std::vector<aiQuatKey> rotationKeys;
		std::vector<aiVectorKey> scalingKeys;
	};
	Animation(aiAnimation& anim);
	const std::unordered_map<std::string, Channel>& GetNameChannelMap() const noexcept;
private:
	std::unordered_map<std::string, Channel> channels;
};