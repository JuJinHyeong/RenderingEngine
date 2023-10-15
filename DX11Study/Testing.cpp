#include "Testing.h"
#include "Vertex.h"
#include "ExtendedXMMath.h"
#include "BindableCommon.h"
#include "RenderTarget.h"
#include "Surface.h"
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

void TestDynamicMeshLoading() {
	using namespace custom;
	Assimp::Importer imp;

	const auto pScene = imp.ReadFile("Models/brick_wall/brick_wall.obj",
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);
	auto layout = VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Tangent)
		.Append(VertexLayout::Bitangent)
		.Append(VertexLayout::Texture2D);
	VertexBuffer buf{ std::move(layout), *pScene->mMeshes[0] };

	for (auto i = 0ull, end = buf.Size(); i < end; i++) {
		const auto a = buf[i].Attr<VertexLayout::Position3D>();
		const auto b = buf[i].Attr<VertexLayout::Normal>();
		const auto c = buf[i].Attr<VertexLayout::Tangent>();
		const auto d = buf[i].Attr<VertexLayout::Bitangent>();
		const auto e = buf[i].Attr<VertexLayout::Texture2D>();
	}
}


void TestScaleMatrixTranslation()
{
	auto tlMat = DirectX::XMMatrixTranslation(20.f, 30.f, 40.f);
	tlMat = ScaleTranslation(tlMat, 0.1f);
	DirectX::XMFLOAT4X4 f4;
	DirectX::XMStoreFloat4x4(&f4, tlMat);
	auto etl = ExtractTranslation(f4);
	assert(etl.x == 2.f && etl.y == 3.f && etl.z == 4.f);
}


void D3DTestScratchPad(Window& wnd) {
	namespace dx = DirectX;
	using namespace custom;

	const auto RenderWithVS = [&gfx = wnd.Gfx()](const std::string& vsName) {
		const auto bitop = Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		const auto layout = VertexLayout{}
			.Append(VertexLayout::Position2D)
			.Append(VertexLayout::Float3Color);

		VertexBuffer vb(layout, 3);
		vb[0].Attr<VertexLayout::Position2D>() = dx::XMFLOAT2{ 0.0f,0.5f };
		vb[0].Attr<VertexLayout::Float3Color>() = dx::XMFLOAT3{ 1.0f,0.0f,0.0f };
		vb[1].Attr<VertexLayout::Position2D>() = dx::XMFLOAT2{ 0.5f,-0.5f };
		vb[1].Attr<VertexLayout::Float3Color>() = dx::XMFLOAT3{ 0.0f,1.0f,0.0f };
		vb[2].Attr<VertexLayout::Position2D>() = dx::XMFLOAT2{ -0.5f,-0.5f };
		vb[2].Attr<VertexLayout::Float3Color>() = dx::XMFLOAT3{ 0.0f,0.0f,1.0f };
		const auto bivb = Bind::VertexBuffer::Resolve(gfx, "##?", vb);

		const std::vector<unsigned short> idx = { 0,1,2 };
		const auto biidx = Bind::IndexBuffer::Resolve(gfx, "##?", idx);

		const auto bips = Bind::PixelShader::Resolve(gfx, "TestPS.cso");

		const auto bivs = Bind::VertexShader::Resolve(gfx, vsName);
		const auto bilay = Bind::InputLayout::Resolve(gfx, layout, *bivs);

		auto rt = Bind::ShaderInputRenderTarget{ gfx,1280,720,0 };

		biidx->Bind(gfx);
		bivb->Bind(gfx);
		bitop->Bind(gfx);
		bips->Bind(gfx);
		bivs->Bind(gfx);
		bilay->Bind(gfx);
		rt.Clear(gfx, { 0.0f,0.0f,0.0f,1.0f });
		rt.BindAsBuffer(gfx);
		gfx.DrawIndexed(biidx->GetCount());
		gfx.GetTarget()->BindAsBuffer(gfx);
		rt.ToSurface(gfx).Save("Test_" + vsName + ".png");
	};

	RenderWithVS("Test1VS.cso");
	RenderWithVS("Test2VS.cso");
}

// testing assimp

// bone testing
class VertexBone {
public:
	VertexBone() {};
	void AddBoneData(unsigned int boneId, float weight) {
		for (unsigned int i = 0; i < 4; i++) {
			// find the first empty slot
			if (weights[i] == 0.0f) {
				boneIds[i] = boneId;
				weights[i] = weight;
				std::stringstream ss;
				ss << "Bone " << boneId << " weight " << weight << " index " << i << "\n";
				OutputDebugString(ss.str().c_str());
				return;
			}
		}
		assert(0 && "Too many bones!");
	}

private:
	unsigned int boneIds[4] = { 0 };
	float weights[4] = { 0.0f };
};

std::vector<VertexBone> vertexToBones;
std::vector<int> meshBaseVertex;
std::map<std::string, unsigned int> boneNameToIndexMap;

int GetBoneId(const aiBone* pBone) {
	unsigned int boneId = 0;
	std::string boneName(pBone->mName.C_Str());

	if (boneNameToIndexMap.find(boneName) == boneNameToIndexMap.end()) {
		boneId = boneNameToIndexMap.size();
		boneNameToIndexMap[boneName] = boneId;
	}
	else {
		boneId = boneNameToIndexMap[boneName];
	}
	return boneId;
}

void ParseSingleBone(int meshIndex, const aiBone* pBone) {
	std::stringstream ss;
	printf("      Bone '%s': num vertices affected by this bone: %d\n", pBone->mName.C_Str(), pBone->mNumWeights);
	int boneId = GetBoneId(pBone);
	ss << "Mesh  " << meshIndex << ": " << "Bone " << pBone->mName.C_Str() << ", " << boneId << " affect to " << pBone->mNumWeights << " number of vertices";
	OutputDebugString(ss.str().c_str());

	for (unsigned int i = 0; i < pBone->mNumWeights; i++) {
		if (i == 0) OutputDebugString("\n");
		const aiVertexWeight& vw = pBone->mWeights[i];

		unsigned int globalVertexId = meshBaseVertex[meshIndex] + vw.mVertexId;

		std::stringstream ss2;
		ss2 << "global Vertex id: " << globalVertexId << " Weight: " << vw.mWeight << " ";
		OutputDebugString(ss2.str().c_str());

		vertexToBones[globalVertexId].AddBoneData(boneId, vw.mWeight);
	}
	OutputDebugString("\n");
}

void ParseMeshBones(int meshIndex, const aiMesh* pMesh) {
	for (unsigned int i = 0; i < pMesh->mNumBones; i++) {
		ParseSingleBone(meshIndex, pMesh->mBones[i]);
	}
}

void ParseMesh(const aiScene* pScene) {
	OutputDebugString("*******************************************************\n");
	std::stringstream ss;
	ss << "Parsing " << pScene->mNumMeshes << " meshes\n";
	OutputDebugString(ss.str().c_str());
	ss.flush();

	unsigned int totalVerticesCount = 0;
	unsigned int totalIndicesCount = 0;
	unsigned int totalBonesCount = 0;

	meshBaseVertex.resize(pScene->mNumMeshes);

	for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
		const aiMesh* pMesh = pScene->mMeshes[i];
		unsigned int numVertices = pMesh->mNumVertices;
		unsigned int numIndices = pMesh->mNumFaces * 3;
		unsigned int numBones = pMesh->mNumBones;

		meshBaseVertex[i] = totalVerticesCount;

		std::stringstream ss2;
		ss2 << "  Mesh " << i << " '" << pMesh->mName.C_Str() << "': vertices " << numVertices << " indices " << numIndices << " bones " << numBones << "\n\n";
		OutputDebugString(ss2.str().c_str());
		totalVerticesCount += numVertices;
		totalIndicesCount += numIndices;
		totalBonesCount += numBones;

		vertexToBones.resize(totalVerticesCount);

		if (pMesh->HasBones()) {
			ParseMeshBones(i, pMesh);
		}
	}

	ss << "Total vertices: " << totalVerticesCount << " total indices: " << totalIndicesCount << " total bones: " << totalBonesCount << "\n";
	OutputDebugString(ss.str().c_str());
}

void AssimpTest(const std::string& filename) {
	Assimp::Importer imp;
	const aiScene* pScene = imp.ReadFile(filename, 
		aiProcess_Triangulate 
		| aiProcess_GenNormals 
		| aiProcess_JoinIdenticalVertices 
		| aiProcess_ConvertToLeftHanded
	);

	if (!pScene) {
		OutputDebugString("Read File Failed\n");
		return;
	}
	ParseMesh(pScene);
}
