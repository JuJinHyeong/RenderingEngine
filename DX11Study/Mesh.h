#pragma once
#include "Drawable.h"
#include "Material.h"
#include "JsonSerializable.h"
#include <span>
#include <array>
#include <unordered_map>
#include <vector>

class Mesh : public Drawable, public JsonSerializable {
	friend class MP;
public:
	Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noexcept(!IS_DEBUG);
	Mesh(Graphics& gfx, std::shared_ptr<Material> matPtr, const aiMesh& mesh, float scale = 1.0f) noexcept(IS_DEBUG);
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs);
	//void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void SetMaterial(Graphics& gfx, const Material& mat, const float scale = 1.0f) noexcept(!IS_DEBUG);

	json ToJson() const noexcept override;

	const std::vector<DirectX::XMFLOAT3>& GetVertices() const noexcept;
	const std::vector<DirectX::XMFLOAT3>& GetTextureCoords() const noexcept;
	const std::vector<DirectX::XMFLOAT3>& GetNormals() const noexcept;
	const std::vector<DirectX::XMFLOAT3>& GetTangents() const noexcept;
	const std::vector<DirectX::XMFLOAT3>& GetBitangents() const noexcept;
	const std::vector<DirectX::XMFLOAT4>& GetColors() const noexcept;
	const std::shared_ptr<Material>& GetMaterialPtr() const noexcept;

private:
	void InitializePerVertexData(const aiMesh& mesh) noexcept(!IS_DEBUG);
	mutable DirectX::XMFLOAT4X4 transform = DirectX::XMFLOAT4X4();
	std::string name;
	std::string tag;

	std::vector<DirectX::XMFLOAT3> vertices;
	std::vector<DirectX::XMFLOAT3> textureCoords;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT3> tangents;
	std::vector<DirectX::XMFLOAT3> bitangents;
	std::vector<DirectX::XMFLOAT4> colors;
	std::vector<unsigned short> indices;

	std::shared_ptr<Material> matPtr;
};
