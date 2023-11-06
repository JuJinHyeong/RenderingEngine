#pragma once
#include "Drawable.h"
#include "Material2.h"
#include "Bone.h"
#include <span>
#include <array>
#include <unordered_map>
#include <vector>

class Mesh : public Drawable {
	friend class MP;
public:
	using string_to_uint_map = std::unordered_map<std::string, unsigned int>;
	Mesh(Graphics& gfx, const Material2& mat, const aiMesh& mesh, const string_to_uint_map& boneNameToIndex, std::vector<Bone>* boneMatrixes, float scale = 1.0f) noexcept(!IS_DEBUG);
	Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noexcept(!IS_DEBUG);
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs);
	//void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	const std::vector<DirectX::XMFLOAT4X4> GetBoneTransforms() const noexcept override;
	void Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);

	const std::vector<DirectX::XMFLOAT3>& GetVertices() const noexcept;
	const std::vector<DirectX::XMFLOAT3>& GetTextureCoords() const noexcept;
	const std::vector<DirectX::XMFLOAT3>& GetNormals() const noexcept;
	const std::vector<DirectX::XMFLOAT3>& GetTangents() const noexcept;
	const std::vector<DirectX::XMFLOAT3>& GetBitangents() const noexcept;
	const std::vector<DirectX::XMFLOAT4>& GetColors() const noexcept;
	const std::vector<std::array<unsigned int, 4>>& GetBoneIndex() const noexcept;
	const std::vector<std::array<float, 4>>& GetBoneWeight() const noexcept;

private:
	void InitializePerVertexData(const aiMesh& mesh, const string_to_uint_map& boneNameToIndex) noexcept(!IS_DEBUG);
	mutable DirectX::XMFLOAT4X4 transform = DirectX::XMFLOAT4X4();
	mutable std::vector<Bone>* boneMatrixesPtr = nullptr;
	std::string name;
	std::string tag;

	std::vector<DirectX::XMFLOAT3> vertices;
	std::vector<DirectX::XMFLOAT3> textureCoords;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT3> tangents;
	std::vector<DirectX::XMFLOAT3> bitangents;
	std::vector<DirectX::XMFLOAT4> colors;
	std::vector<unsigned short> indices;

	std::vector<std::array<unsigned int, 4>> boneIndex;
	std::vector<std::array<float, 4>> boneWeight;
};
