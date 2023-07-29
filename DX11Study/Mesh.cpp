#include "Mesh.h"
#include "imgui/imgui.h"
#include "Texture.h"
#include "Surface.h"
#include "Sampler.h"
#include <unordered_map>
#include <sstream>

ModelException::ModelException(int line, const char* file, std::string note) noexcept
	:
	BasicException(line, file),
	note(std::move(note)) {}

const char* ModelException::what() const noexcept {
	std::ostringstream oss;
	oss << BasicException::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept {
	return "Model Exception";
}

const std::string& ModelException::GetNote() const noexcept {
	return note;
}

// Mesh
Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs) {
	if (!IsStaticInitialized()) {
		AddStaticBind(std::make_unique<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	for (auto& pb : bindPtrs) {
		if (auto pi = dynamic_cast<Bind::IndexBuffer*>(pb.get())) {
			AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer>{ pi });
			pb.release();
		}
		else {
			AddBind(std::move(pb));
		}
	}

	AddBind(std::make_unique<Bind::TransformCbuf>(gfx, *this));
}
void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG) {
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}
DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept {
	return DirectX::XMLoadFloat4x4(&transform);
}


// Node
Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG)
	:
	meshPtrs(std::move(meshPtrs)),
	name(name),
	id(id) {
	DirectX::XMStoreFloat4x4(&this->transform, transform);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
}
void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG) {
	const auto built = DirectX::XMLoadFloat4x4(&appliedTransform)
		* DirectX::XMLoadFloat4x4(&transform)
		* accumulatedTransform;

	for (const auto pm : meshPtrs) {
		pm->Draw(gfx, built);
	}
	for (const auto& pc : childPtrs) {
		pc->Draw(gfx, built);
	}
}

int Node::GetId() const noexcept {
	return id;
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG) {
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}
void Node::ShowTree(Node*& pSelectedNode) const noexcept(!IS_DEBUG) {
	// compare with this can't be??
	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
	const int nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
		| (pSelectedNode == this ? ImGuiTreeNodeFlags_Selected : 0)
		| (childPtrs.empty() ? ImGuiTreeNodeFlags_Leaf : 0);

	// create expandable node
	const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), nodeFlags, name.c_str());

	if (ImGui::IsItemClicked()) {
		// future need to modify
		pSelectedNode = const_cast<Node*>(this);
	}
	// int to void* need to cast.
	if (expanded) {
		for (const auto& pChild : childPtrs) {
			pChild->ShowTree(pSelectedNode);
		}
		ImGui::TreePop();
	}
}
void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG) {
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}

// Model

// ModelWindow class doesn't need to be read from another header.
// so ModelWindow class is defined in cpp file.
class ModelWindow {
public:
	void Show(const char* windowName, const Node& root) noexcept(!IS_DEBUG) {
		windowName = windowName ? windowName : "Model";
		if (ImGui::Begin(windowName)) {
			ImGui::Columns(2, nullptr, true);
			root.ShowTree(pSelectedNode);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr) {
				ImGui::Text("Id: %d", pSelectedNode->GetId());
				auto& transform = transforms[pSelectedNode->GetId()];
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);

				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
			}
		}
		ImGui::End();
	}
	DirectX::XMMATRIX GetTransform() const noexcept(!IS_DEBUG) {
		assert(pSelectedNode != nullptr);
		const auto& transform = transforms.at(pSelectedNode->GetId());
		return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw)
			* DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}
	Node* GetSelectedNode() const noexcept(!IS_DEBUG) {
		return pSelectedNode;
	}
private:
	Node* pSelectedNode = nullptr;
	struct TransformParameters {
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> transforms;
};

Model::Model(Graphics& gfx, const std::string fileName)
	:
	pWindow(std::make_unique<ModelWindow>()) {
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals
	);

	if (pScene == nullptr) {
		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
	}

	for (size_t i = 0; i < pScene->mNumMeshes; i++) {
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));
	}

	int curId = 0;
	pRoot = ParseNode(curId, *pScene->mRootNode);
}

Model::~Model() noexcept(!IS_DEBUG) {}

void Model::Draw(Graphics& gfx) const noexcept(!IS_DEBUG) {
	if (auto node = pWindow->GetSelectedNode()) {
		node->SetAppliedTransform(pWindow->GetTransform());
	}
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}
void Model::ShowWindow(const char* windowName) noexcept(!IS_DEBUG) {
	pWindow->Show(windowName, *pRoot);
}
std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials) {
	using custom::VertexLayout;

	custom::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Texture2D)
	));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++) {
		vbuf.EmplaceBack(
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
		);
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	std::vector<std::unique_ptr<Bind::Bindable>> bindablePtrs;

	// i think this variable change to int and use switch statement (no texture, diffuse, diffuse + specular)
	bool hasSpecularMap = false;
	float shiniess = 35.0f;
	// add material ex) texture, specular, sampler...
	if (mesh.mMaterialIndex >= 0) {
		auto& material = *pMaterials[mesh.mMaterialIndex];
		const std::string base = std::string("models/nano_textured/");
		aiString texFileName;

		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS) {
			bindablePtrs.push_back(std::make_unique<Bind::Texture>(gfx, Surface::FromFile(base + texFileName.C_Str())));
		}

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS) {
			bindablePtrs.push_back(std::make_unique<Bind::Texture>(gfx, Surface::FromFile(base + texFileName.C_Str()), 1u));
			hasSpecularMap = true;
		}
		else {
			material.Get(AI_MATKEY_SHININESS, shiniess);
		}

		bindablePtrs.push_back(std::make_unique<Bind::Sampler>(gfx));
	}

	bindablePtrs.push_back(std::make_unique<Bind::VertexBuffer>(gfx, vbuf));

	bindablePtrs.push_back(std::make_unique<Bind::IndexBuffer>(gfx, indices));

	auto pvs = std::make_unique<Bind::VertexShader>(gfx, L"PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));

	bindablePtrs.push_back(std::make_unique<Bind::InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

	if (hasSpecularMap) {
		bindablePtrs.push_back(std::make_unique<Bind::PixelShader>(gfx, L"PhongSpecMapPS.cso"));
	}
	else {
		bindablePtrs.push_back(std::make_unique<Bind::PixelShader>(gfx, L"PhongPS.cso"));

		struct PSMaterialConstant {
			//DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
			float specularIntensity = 1.6f;
			float specularPower = 0.0f;
			float padding[2] = { 0.0f, 0.0f };
		} pmc;
		pmc.specularPower = shiniess;
		bindablePtrs.push_back(std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
	}

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(int& curId, const aiNode& node) {
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++) {
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(curId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++) {
		pNode->AddChild(ParseNode(curId, *node.mChildren[i]));
	}

	return pNode;
}