#include "Animation.h"

Animation::Animation(aiAnimation& anim) {
	for (unsigned int i = 0; i < anim.mNumChannels; i++) {
		aiNodeAnim* channel = anim.mChannels[i];
		channels[channel->mNodeName.C_Str()].positionKeys = std::vector<aiVectorKey>(channel->mPositionKeys, channel->mPositionKeys + channel->mNumPositionKeys);
		channels[channel->mNodeName.C_Str()].rotationKeys = std::vector<aiQuatKey>(channel->mRotationKeys, channel->mRotationKeys + channel->mNumRotationKeys);
		channels[channel->mNodeName.C_Str()].scalingKeys = std::vector<aiVectorKey>(channel->mScalingKeys, channel->mScalingKeys + channel->mNumScalingKeys);
	}
}

const std::unordered_map<std::string, Animation::Channel>& Animation::GetNameChannelMap() const noexcept {
	return channels;
}
