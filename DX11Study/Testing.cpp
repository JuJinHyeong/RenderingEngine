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
#include <fstream>
#include "json.hpp"
#include "Scene.h"
#include "SceneObject.h"
#include "Model.h"
#include "CustomMath.h"

using namespace DirectX;

#define DumpToFile(...) Dump::WriteToFile("test_parse.txt", __VA_ARGS__);

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

#pragma region bonetest

#define MAX_NUM_BONES_PER_VERTEX 4

struct BoneInfo {
	DirectX::XMFLOAT4X4 boneOffset = DirectX::XMFLOAT4X4();
	DirectX::XMFLOAT4X4 finalTransformation = DirectX::XMFLOAT4X4();

	BoneInfo(const aiMatrix4x4& offset) {
		boneOffset = *reinterpret_cast<const DirectX::XMFLOAT4X4*>(&offset);
	}
};

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

std::vector<BoneInfo> bone_info;
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
		bone_info.emplace_back(pBone->mOffsetMatrix);
	}
	else {
		bone_id = bone_name_to_index_map[bone_name];
	}

	return bone_id;
}

void parse_single_bone(int mesh_index, const aiBone* pBone) {
	int bone_id = get_bone_id(pBone);
	DumpToFile("#", bone_id, "Bone ", pBone->mName.C_Str(), ": num vertices affected by this bone: ", pBone->mNumWeights, "\n");
	//    printf("      Bone id %d\n", bone_id);

	//auto mat = reinterpret_cast<const DirectX::XMFLOAT4X4*>(&pBone->mOffsetMatrix);
	//std::string matStr = Dump::MatrixToString(pBone->mOffsetMatrix);
	//DumpToFile(matStr, '\n');

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
	DumpToFile("Parsing ", pScene->mNumMeshes, " meshes\n");

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
		DumpToFile("\nMesh ", i, " '", pMesh->mName.C_Str(), "': vertices ", num_vertices, " indices ", num_indices, " bones ", num_bones, "\n\n");
		total_vertices += num_vertices;
		total_indices += num_indices;
		total_bones += num_bones;

		vertex_to_bones.resize(total_vertices);

		if (pMesh->HasBones()) {
			parse_mesh_bones(i, pMesh);
		}
	}

	DumpToFile("\nTotal bones: ", bone_info.size(), "\n");
	for (auto pair : bone_name_to_index_map) {
		DumpToFile("#", pair.second, ":", pair.first, "\n");
	}
}

XMMATRIX inverseRootTransform = XMMatrixIdentity();
void parse_node(const aiNode* pNode, const FXMMATRIX& parentTransform) {
	auto transform = (DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&pNode->mTransformation)
	)));

	const auto accumulatedTransform = transform * parentTransform;

	if (bone_name_to_index_map.find(pNode->mName.C_Str()) != bone_name_to_index_map.end()) {
		unsigned int boneIndex = bone_name_to_index_map[pNode->mName.C_Str()];
		auto finalTransformation = XMMatrixTranspose(XMLoadFloat4x4(&bone_info[boneIndex].boneOffset)) * accumulatedTransform * inverseRootTransform;
		XMStoreFloat4x4(&bone_info[boneIndex].finalTransformation, finalTransformation);
	}

	std::string pad(space_count, ' ');
	DumpToFile(pad, "Node name: '", pNode->mName.C_Str(), "' num children ", pNode->mNumChildren, " num meshes ", pNode->mNumMeshes, "\n");
	DumpToFile(pad, "Transformation:\n");
	auto matStr2 = Dump::MatrixToString(XMMatrixTranspose(transform), space_count);
	DumpToFile(matStr2);

	DumpToFile(pad, "Accmulated Transformation:\n");
	//auto mat = reinterpret_cast<const DirectX::XMFLOAT4X4*>(&pNode->mTransformation);
	auto matStr = Dump::MatrixToString(XMMatrixTranspose(accumulatedTransform), space_count);
	DumpToFile(matStr);

	space_count += 4;

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		std::string pad2(space_count, ' ');
		DumpToFile("\n");
		DumpToFile(pad2, "--- ", i, " ---\n");
		parse_node(pNode->mChildren[i], accumulatedTransform);
	}

	space_count -= 4;
}


void parse_hierarchy(const aiScene* pScene) {
	DumpToFile("*******************************************************\n");
	DumpToFile("Parsing Nodes\n");

	parse_node(pScene->mRootNode, DirectX::XMMatrixIdentity());

	DumpToFile("\nTotal bones: ", bone_info.size(), "\n");
	for (auto pair : bone_name_to_index_map) {
		DumpToFile("#", pair.second, ":", pair.first, "\n");
	}
	DumpToFile("\n");
	for (int i = 0; i < bone_info.size(); i++) {
		DumpToFile("#", i, "\n");
		std::string matStr = Dump::MatrixToString(bone_info[i].boneOffset);
		DumpToFile("offset\n", matStr);
		std::string matStr2 = Dump::MatrixToString(bone_info[i].finalTransformation);
		DumpToFile("final\n", matStr2, "\n");
	}
}

void parse_scene(const aiScene* pScene) {
	parse_meshes(pScene);

	auto rootTransform = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&pScene->mRootNode->mTransformation));
	XMVECTOR determinant = XMMatrixDeterminant(rootTransform);
	inverseRootTransform = XMMatrixTranspose(XMMatrixInverse(&determinant, rootTransform));
	parse_hierarchy(pScene);
}

void AssimpTest(const std::string& filename, float tick) {
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
#pragma endregion

void JsonTest()
{
	using json = nlohmann::json;
	json j;
	j["int"] = 1;
	j["float"] = 1.3f;
	j["string"] = std::string("this is test");
	j["bool"] = true;
	j["array constructor"] = json::array({1,2,3,4,5});
	j["array push_back"] = json::array();
	j["array push_back"].push_back(1);
	j["array push_back"].push_back(2);
	j["array push_back"].push_back(3);
	j["array push_back"].push_back(4);
	j["array push_back"].push_back(5);
	json nested;
	nested["int"] = 123;
	nested["array"] = json::array({ 5,4,3,2,1 });
	j["nested json"] = nested;
	
	std::ofstream out("test.json");
	if (out.is_open()) {
		out << j.dump(2);
	}
	else {
		out << "error!";
	}
	out.close();
}

void SceneTest(Graphics& gfx)
{
	using json = nlohmann::json;
	Scene scene("test");
	auto obj1 = std::make_unique<SceneObject>("test1");
	auto nano = std::make_unique<Model>(gfx, "models/nano_textured/nanosuit.obj", 1.0f);
	auto nanoTf = DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(2.0f, 1.0f, 2.0f) 
		* DirectX::XMMatrixRotationX(PI / 2)
		* DirectX::XMMatrixRotationY(PI / 2)
		* DirectX::XMMatrixRotationZ(0.0f)
		* DirectX::XMMatrixTranslation(10.0f, -1.0f, 3.2f));
	nano->SetRootTransform(nanoTf);
	obj1->SetObject(std::move(nano));
	auto obj2 = std::make_unique<SceneObject>("test2");
	obj2->SetObject(std::make_unique<Model>(gfx, "models/gobber/GoblinX.obj", 1.0f));
	scene.AddObject(std::move(obj1));
	scene.AddObject(std::move(obj2));
	
	json j = scene.ToJson();
	std::ofstream out("scenetest.json");
	if (out.is_open()) {
		out << j.dump(2);
	}
	else {
		out << "error!";
	}
	out.close();
}
