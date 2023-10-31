#pragma once
#include "Drawable.h"
#include "Material2.h"
#include "Bone.h"
#include <span>
#include <array>
#include <unordered_map>
#include <vector>

class Mesh : public Drawable {
public:
	Mesh(Graphics& gfx, const Material2& mat, const aiMesh& mesh, float scale = 1.0f) noexcept(!IS_DEBUG);
	Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noexcept(!IS_DEBUG);
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs);
	//void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);

	const std::span<aiVector3D>& GetVertices() const noexcept;
	const std::span<aiVector3D>& GetTextureCoords() const noexcept;
	const std::span<aiVector3D>& GetNormals() const noexcept;
	const std::span<aiVector3D>& GetTangents() const noexcept;
	const std::span<aiVector3D>& GetBitangents() const noexcept;
	const std::span<aiColor4D>& GetColors() const noexcept;
	const std::span<aiFace>& GetFaces() const noexcept;
	const std::span<aiBone*>& GetBones() const noexcept;
	const std::vector<std::array<unsigned int, 4>>& GetBoneIndex() const noexcept;
	const std::vector<std::array<float, 4>>& GetBoneWeight() const noexcept;

private:
	mutable DirectX::XMFLOAT4X4 transform = DirectX::XMFLOAT4X4();
	std::string name;
	std::string tag;

	std::span<aiVector3D> vertices;
	std::span<aiVector3D> textureCoords;
	std::span<aiVector3D> normals;
	std::span<aiVector3D> tangents;
	std::span<aiVector3D> bitangents;
	std::span<aiColor4D> colors;
	std::span<aiFace> faces;
	std::span<aiBone*> bones;
	std::vector<std::array<unsigned int, 4>> boneIndex;
	std::vector<std::array<float, 4>> boneWeight;
};
