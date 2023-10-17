#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include "Material2.h"
#include "Bone.h"

class Mesh : public Drawable {
public:
	Mesh(Graphics& gfx, const Material2& mat, const aiMesh& mesh, float scale = 1.0f) noexcept(!IS_DEBUG);
	Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noexcept(!IS_DEBUG);
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs);
	//void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
private:
	void SetBones(const aiMesh& mesh) noexcept(!IS_DEBUG);
	void SetTechnique(Graphics& gfx, const Material2& mat, const aiMesh& mesh, float scale = 1.0f) noexcept(!IS_DEBUG);
	mutable DirectX::XMFLOAT4X4 transform = DirectX::XMFLOAT4X4();
	std::string tag;
	std::vector<std::shared_ptr<Bone>> bones;
};
