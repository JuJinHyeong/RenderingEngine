#include "AssimpTest.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Vertex.h"

AssimpTest::AssimpTest(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	DirectX::XMFLOAT3 material,
	float scale)
	:
	TestObject(gfx, rng, adist, ddist, odist, rdist) {
	namespace dx = DirectX;

	if (!IsStaticInitialized()) {
		struct Vertex {
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
		};
		using custom::VertexLayout;
		custom::VertexBuffer vbuf(std::move(VertexLayout{}.Append(VertexLayout::Position3D).Append(VertexLayout::Normal)));

		Assimp::Importer imp;
		const auto pModel = imp.ReadFile("models/suzanne.obj",
			aiProcess_JoinIdenticalVertices
		);
		const auto pMesh = pModel->mMeshes[0];

		std::vector<Vertex> vertices;
		vertices.reserve(pMesh->mNumVertices);
		for (unsigned int i = 0; i < pMesh->mNumVertices; i++) {
			vbuf.EmplaceBack(dx::XMFLOAT3{ pMesh->mVertices[i].x* scale, pMesh->mVertices[i].y* scale, pMesh->mVertices[i].z* scale },
				*reinterpret_cast<dx::XMFLOAT3*>(&pMesh->mNormals[i]));
		}

		std::vector<unsigned short> indices;
		indices.reserve(pMesh->mNumFaces * 3);
		for (unsigned int i = 0; i < pMesh->mNumFaces; i++) {
			const auto& face = pMesh->mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		AddStaticBind(std::make_unique<Bind::VertexBuffer>(gfx, vbuf));

		AddStaticIndexBuffer(std::make_unique<Bind::IndexBuffer>(gfx, indices));

		auto pvs = std::make_unique<Bind::VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<Bind::PixelShader>(gfx, L"PhongPS.cso"));

		AddStaticBind(std::make_unique<Bind::InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));
		AddStaticBind(std::make_unique<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant {
			DirectX::XMFLOAT3 color = { 0.0f, 0.0f, 0.0f };
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[3] = { 0.0f };
		} pmc;
		pmc.color = material;
		AddStaticBind(std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
	}
	else {
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<Bind::TransformCbuf>(gfx, *this));
}