#include "Testing.h"
#include "ExtendedXMMath.h"
#include "BindableCommon.h"
#include "RenderTarget.h"
#include "Surface.h"
#include "Dump.h"
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <stdio.h>

template<typename... Args>
void DumpToFile(const Args&... args) {
	Dump::WriteToFile("test_parse.txt", args...);
}
//
//void TestScaleMatrixTranslation()
//{
//	auto tlMat = DirectX::XMMatrixTranslation(20.f, 30.f, 40.f);
//	tlMat = ScaleTranslation(tlMat, 0.1f);
//	DirectX::XMFLOAT4X4 f4;
//	DirectX::XMStoreFloat4x4(&f4, tlMat);
//	auto etl = ExtractTranslation(f4);
//	assert(etl.x == 2.f && etl.y == 3.f && etl.z == 4.f);
//}
//void D3DTestScratchPad(Window& wnd) {
//	namespace dx = DirectX;
//	using namespace custom;
//
//	const auto RenderWithVS = [&gfx = wnd.Gfx()](const std::string& vsName) {
//		const auto bitop = Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//		const auto layout = VertexLayout{}
//			.Append(VertexLayout::Position2D)
//			.Append(VertexLayout::Float3Color);
//
//		VertexBuffer vb(layout, 3);
//		vb[0].Attr<VertexLayout::Position2D>() = dx::XMFLOAT2{ 0.0f,0.5f };
//		vb[0].Attr<VertexLayout::Float3Color>() = dx::XMFLOAT3{ 1.0f,0.0f,0.0f };
//		vb[1].Attr<VertexLayout::Position2D>() = dx::XMFLOAT2{ 0.5f,-0.5f };
//		vb[1].Attr<VertexLayout::Float3Color>() = dx::XMFLOAT3{ 0.0f,1.0f,0.0f };
//		vb[2].Attr<VertexLayout::Position2D>() = dx::XMFLOAT2{ -0.5f,-0.5f };
//		vb[2].Attr<VertexLayout::Float3Color>() = dx::XMFLOAT3{ 0.0f,0.0f,1.0f };
//		const auto bivb = Bind::VertexBuffer::Resolve(gfx, "##?", vb);
//
//		const std::vector<unsigned short> idx = { 0,1,2 };
//		const auto biidx = Bind::IndexBuffer::Resolve(gfx, "##?", idx);
//
//		const auto bips = Bind::PixelShader::Resolve(gfx, "TestPS.cso");
//
//		const auto bivs = Bind::VertexShader::Resolve(gfx, vsName);
//		const auto bilay = Bind::InputLayout::Resolve(gfx, layout, *bivs);
//
//		auto rt = Bind::ShaderInputRenderTarget{ gfx,1280,720,0 };
//
//		biidx->Bind(gfx);
//		bivb->Bind(gfx);
//		bitop->Bind(gfx);
//		bips->Bind(gfx);
//		bivs->Bind(gfx);
//		bilay->Bind(gfx);
//		rt.Clear(gfx, { 0.0f,0.0f,0.0f,1.0f });
//		rt.BindAsBuffer(gfx);
//		gfx.DrawIndexed(biidx->GetCount());
//		gfx.GetTarget()->BindAsBuffer(gfx);
//		rt.ToSurface(gfx).Save("Test_" + vsName + ".png");
//	};
//
//	RenderWithVS("Test1VS.cso");
//	RenderWithVS("Test2VS.cso");
//}

// testing assimp
#define MAX_NUM_BONES_PER_VERTEX 4

struct VertexBoneData {
	unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
	float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };

	VertexBoneData() {}

	void AddBoneData(unsigned int BoneID, float Weight) {
		for (unsigned int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
			if (Weights[i] == 0.0) {
				BoneIDs[i] = BoneID;
				Weights[i] = Weight;
				return;
			}
		}
		assert(0);
	}
};


std::vector<VertexBoneData> vertex_to_bones;
std::vector<int> mesh_base_vertex;
std::map<std::string, unsigned int> bone_name_to_index_map;

static int space_count = 0;
float Round(float num) {
	return std::round(num * 100.0f) / 100.0f;
}

int get_bone_id(const aiBone* pBone) {
	int bone_id = 0;
	std::string bone_name(pBone->mName.C_Str());

	if (bone_name_to_index_map.find(bone_name) == bone_name_to_index_map.end()) {
		// Allocate an index for a new bone
		bone_id = (int)bone_name_to_index_map.size();
		bone_name_to_index_map[bone_name] = bone_id;
	}
	else {
		bone_id = bone_name_to_index_map[bone_name];
	}

	return bone_id;
}

void parse_single_bone(int mesh_index, const aiBone* pBone) {
	DumpToFile("Bone ", pBone->mName.C_Str(), ": num vertices affected by this bone: ", pBone->mNumWeights, "\n");

	int bone_id = get_bone_id(pBone);
	//    printf("      Bone id %d\n", bone_id);

	//auto mat = reinterpret_cast<const DirectX::XMFLOAT4X4*>(&pBone->mOffsetMatrix);
	std::string matStr = Dump::MatrixToString(pBone->mOffsetMatrix);
	DumpToFile(matStr, '\n');

	for (unsigned int i = 0; i < pBone->mNumWeights; i++) {
		//        if (i == 0) printf("\n");
		const aiVertexWeight& vw = pBone->mWeights[i];
		//          printf("       %d: vertex id %d weight %.2f\n", i, vw.mVertexId, vw.mWeight);

		unsigned int global_vertex_id = mesh_base_vertex[mesh_index] + vw.mVertexId;
		//        printf("Vertex id %d ", global_vertex_id);

		assert(global_vertex_id < vertex_to_bones.size());
		vertex_to_bones[global_vertex_id].AddBoneData(bone_id, vw.mWeight);
	}
}


void parse_mesh_bones(int mesh_index, const aiMesh* pMesh) {
	for (unsigned int i = 0; i < pMesh->mNumBones; i++) {
		parse_single_bone(mesh_index, pMesh->mBones[i]);
	}
}


void parse_meshes(const aiScene* pScene) {
	DumpToFile("Parsing ", pScene->mNumMeshes, " meshes\n\n");

	int total_vertices = 0;
	int total_indices = 0;
	int total_bones = 0;

	mesh_base_vertex.resize(pScene->mNumMeshes);

	for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
		const aiMesh* pMesh = pScene->mMeshes[i];
		int num_vertices = pMesh->mNumVertices;
		int num_indices = pMesh->mNumFaces * 3;
		int num_bones = pMesh->mNumBones;
		mesh_base_vertex[i] = total_vertices;
		DumpToFile("Mesh ", i, " '", pMesh->mName.C_Str(), "': vertices ", num_vertices, " indices ", num_indices, " bones ", num_bones, "\n\n");
		total_vertices += num_vertices;
		total_indices += num_indices;
		total_bones += num_bones;

		vertex_to_bones.resize(total_vertices);

		if (pMesh->HasBones()) {
			parse_mesh_bones(i, pMesh);
		}
	}
	//DumpToFile("\nTotal vertices ", total_vertices, " total indices ", total_indices, "total bones ", total_bones);
}


void parse_node(const aiNode* pNode) {
	std::string pad(space_count, ' ');
	DumpToFile(pad, "Node name: '", pNode->mName.C_Str(), "' num children ", pNode->mNumChildren, " num meshes ", pNode->mNumMeshes, "\n");
	DumpToFile(pad, "Node Transformation:\n");
	//auto mat = reinterpret_cast<const DirectX::XMFLOAT4X4*>(&pNode->mTransformation);
	auto matStr = Dump::MatrixToString(pNode->mTransformation, space_count);
	DumpToFile(matStr);

	space_count += 4;

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		std::string pad2(space_count, ' ');
		DumpToFile("\n");
		DumpToFile(pad2, "--- ", i, " ---\n");
		parse_node(pNode->mChildren[i]);
	}

	space_count -= 4;
}


void parse_hierarchy(const aiScene* pScene) {
	DumpToFile("\n*******************************************************\n");
	DumpToFile("Parsing Nodes\n");

	parse_node(pScene->mRootNode);
}


void parse_scene(const aiScene* pScene) {
	parse_meshes(pScene);

	parse_hierarchy(pScene);
}

void AssimpTest(const std::string& filename) {
	Assimp::Importer Importer;
	const aiScene* pScene = Importer.ReadFile(filename,
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ConvertToLeftHanded |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace
	);

	if (!pScene) {
		printf("Error parsing '%s': '%s'\n", filename.c_str(), Importer.GetErrorString());
	}

	Dump::ClearFile("test_parse.txt");
	parse_scene(pScene);

}