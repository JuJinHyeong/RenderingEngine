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
#include <stdio.h>

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
				//printf("Adding bone %d weight %f at index %i\n", BoneID, Weight, i);
				return;
			}
		}

		// should never get here - more bones than we have space for
		assert(0);
	}
};


std::vector<VertexBoneData> vertex_to_bones;
std::vector<int> mesh_base_vertex;
std::map<std::string, unsigned int> bone_name_to_index_map;

static int space_count = 0;

void print_space() {
	for (int i = 0; i < space_count; i++) {
		printf(" ");
	}
}


void print_assimp_matrix(const aiMatrix4x4& m) {
	print_space(); printf("%f %f %f %f\n", m.a1, m.a2, m.a3, m.a4);
	print_space(); printf("%f %f %f %f\n", m.b1, m.b2, m.b3, m.b4);
	print_space(); printf("%f %f %f %f\n", m.c1, m.c2, m.c3, m.c4);
	print_space(); printf("%f %f %f %f\n", m.d1, m.d2, m.d3, m.d4);
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
	printf("      Bone '%s': num vertices affected by this bone: %d\n", pBone->mName.C_Str(), pBone->mNumWeights);

	int bone_id = get_bone_id(pBone);
	//    printf("      Bone id %d\n", bone_id);

	print_assimp_matrix(pBone->mOffsetMatrix);

	for (unsigned int i = 0; i < pBone->mNumWeights; i++) {
		//        if (i == 0) printf("\n");
		const aiVertexWeight& vw = pBone->mWeights[i];
		//          printf("       %d: vertex id %d weight %.2f\n", i, vw.mVertexId, vw.mWeight);

		unsigned int global_vertex_id = mesh_base_vertex[mesh_index] + vw.mVertexId;
		//        printf("Vertex id %d ", global_vertex_id);

		assert(global_vertex_id < vertex_to_bones.size());
		vertex_to_bones[global_vertex_id].AddBoneData(bone_id, vw.mWeight);
	}

	printf("\n");
}


void parse_mesh_bones(int mesh_index, const aiMesh* pMesh) {
	for (unsigned int i = 0; i < pMesh->mNumBones; i++) {
		parse_single_bone(mesh_index, pMesh->mBones[i]);
	}
}


void parse_meshes(const aiScene* pScene) {
	printf("*******************************************************\n");
	printf("Parsing %d meshes\n\n", pScene->mNumMeshes);

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
		printf("  Mesh %d '%s': vertices %d indices %d bones %d\n\n", i, pMesh->mName.C_Str(), num_vertices, num_indices, num_bones);
		total_vertices += num_vertices;
		total_indices += num_indices;
		total_bones += num_bones;

		vertex_to_bones.resize(total_vertices);

		if (pMesh->HasBones()) {
			parse_mesh_bones(i, pMesh);
		}

		printf("\n");
	}

	printf("\nTotal vertices %d total indices %d total bones %d\n", total_vertices, total_indices, total_bones);
}


void parse_node(const aiNode* pNode) {
	print_space(); printf("Node name: '%s' num children %d num meshes %d\n", pNode->mName.C_Str(), pNode->mNumChildren, pNode->mNumMeshes);
	print_space(); printf("Node transformation:\n");
	print_assimp_matrix(pNode->mTransformation);

	space_count += 4;

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		printf("\n");
		print_space(); printf("--- %d ---\n", i);
		parse_node(pNode->mChildren[i]);
	}

	space_count -= 4;
}


void parse_hierarchy(const aiScene* pScene) {
	printf("\n*******************************************************\n");
	printf("Parsing the node hierarchy\n");

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

	parse_scene(pScene);

}
//// bone testing
//class VertexBone {
//public:
//	VertexBone() {};
//	void AddBoneData(unsigned int boneId, float weight) {
//		for (unsigned int i = 0; i < 4; i++) {
//			// find the first empty slot
//			if (weights[i] == 0.0f) {
//				boneIds[i] = boneId;
//				weights[i] = weight;
//				std::stringstream ss;
//				ss << "Bone " << boneId << " weight " << weight << " index " << i << "\n";
//				OutputDebugString(ss.str().c_str());
//				return;
//			}
//		}
//		assert(0 && "Too many bones!");
//	}
//
//private:
//	unsigned int boneIds[4] = { 0 };
//	float weights[4] = { 0.0f };
//};
//
//std::vector<VertexBone> vertexToBones;
//std::vector<int> meshBaseVertex;
//std::map<std::string, unsigned int> boneNameToIndexMap;
//
//int GetBoneId(const aiBone* pBone) {
//	unsigned int boneId = 0;
//	std::string boneName(pBone->mName.C_Str());
//
//	if (boneNameToIndexMap.find(boneName) == boneNameToIndexMap.end()) {
//		boneId = boneNameToIndexMap.size();
//		boneNameToIndexMap[boneName] = boneId;
//	}
//	else {
//		boneId = boneNameToIndexMap[boneName];
//	}
//	return boneId;
//}
//
//void ParseSingleBone(int meshIndex, const aiBone* pBone) {
//	std::stringstream ss;
//	printf("      Bone '%s': num vertices affected by this bone: %d\n", pBone->mName.C_Str(), pBone->mNumWeights);
//	int boneId = GetBoneId(pBone);
//	ss << "Mesh  " << meshIndex << ": " << "Bone " << pBone->mName.C_Str() << ", " << boneId << " affect to " << pBone->mNumWeights << " number of vertices";
//	OutputDebugString(ss.str().c_str());
//
//	for (unsigned int i = 0; i < pBone->mNumWeights; i++) {
//		if (i == 0) OutputDebugString("\n");
//		const aiVertexWeight& vw = pBone->mWeights[i];
//
//		unsigned int globalVertexId = meshBaseVertex[meshIndex] + vw.mVertexId;
//
//		std::stringstream ss2;
//		ss2 << "global Vertex id: " << globalVertexId << " Weight: " << vw.mWeight << " ";
//		OutputDebugString(ss2.str().c_str());
//
//		vertexToBones[globalVertexId].AddBoneData(boneId, vw.mWeight);
//	}
//	OutputDebugString("\n");
//}
//
//void ParseMeshBones(int meshIndex, const aiMesh* pMesh) {
//	for (unsigned int i = 0; i < pMesh->mNumBones; i++) {
//		ParseSingleBone(meshIndex, pMesh->mBones[i]);
//	}
//}
//
//void ParseMesh(const aiScene* pScene) {
//	OutputDebugString("*******************************************************\n");
//	std::stringstream ss;
//	ss << "Parsing " << pScene->mNumMeshes << " meshes\n";
//	OutputDebugString(ss.str().c_str());
//	ss.flush();
//
//	unsigned int totalVerticesCount = 0;
//	unsigned int totalIndicesCount = 0;
//	unsigned int totalBonesCount = 0;
//
//	meshBaseVertex.resize(pScene->mNumMeshes);
//
//	for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
//		const aiMesh* pMesh = pScene->mMeshes[i];
//		unsigned int numVertices = pMesh->mNumVertices;
//		unsigned int numIndices = pMesh->mNumFaces * 3;
//		unsigned int numBones = pMesh->mNumBones;
//
//		meshBaseVertex[i] = totalVerticesCount;
//
//		std::stringstream ss2;
//		ss2 << "  Mesh " << i << " '" << pMesh->mName.C_Str() << "': vertices " << numVertices << " indices " << numIndices << " bones " << numBones << "\n\n";
//		OutputDebugString(ss2.str().c_str());
//		totalVerticesCount += numVertices;
//		totalIndicesCount += numIndices;
//		totalBonesCount += numBones;
//
//		vertexToBones.resize(totalVerticesCount);
//
//		if (pMesh->HasBones()) {
//			ParseMeshBones(i, pMesh);
//		}
//	}
//
//	ss << "Total vertices: " << totalVerticesCount << " total indices: " << totalIndicesCount << " total bones: " << totalBonesCount << "\n";
//	OutputDebugString(ss.str().c_str());
//}
//
//void AssimpTest(const std::string& filename) {
//	Assimp::Importer imp;
//	const aiScene* pScene = imp.ReadFile(filename,
//		aiProcess_Triangulate |
//		aiProcess_JoinIdenticalVertices |
//		aiProcess_ConvertToLeftHanded |
//		aiProcess_GenNormals |
//		aiProcess_CalcTangentSpace
//	);
//
//	if (!pScene) {
//		OutputDebugString("Read File Failed\n");
//		return;
//	}
//	ParseMesh(pScene);
//}
