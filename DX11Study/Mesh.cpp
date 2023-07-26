#include "Mesh.h"
#include "imgui/imgui.h"
#include <unordered_map>
#include <sstream>

ModelException::ModelException(int line, const char* file, std::string note) noexcept
	:
	BasicException(line, file),
	note(std::move(note))
{
}

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
Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtrs) {
	if (!IsStaticInitialized()) {
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	for (auto& pb : bindPtrs) {
		if (auto pi = dynamic_cast<IndexBuffer*>(pb.get())) {
			AddIndexBuffer(std::unique_ptr<IndexBuffer>{ pi });
			pb.release();
		}
		else {
			AddBind(std::move(pb));
		}
	}

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}
void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG) {
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}
DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept {
	return DirectX::XMLoadFloat4x4(&transform);
}


// Node
Node::Node(const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG)
	:
	meshPtrs(std::move(meshPtrs)),
	name(name)
{
	DirectX::XMStoreFloat4x4(&baseTransform, transform);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
}
void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG) {
	const auto built = DirectX::XMLoadFloat4x4(&baseTransform) 
		* DirectX::XMLoadFloat4x4(&appliedTransform) 
		* accumulatedTransform;

	for (const auto pm : meshPtrs) {
		pm->Draw(gfx, built);
	}
	for (const auto& pc : childPtrs) {
		pc->Draw(gfx, built);
	}
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG) {
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}

void Node::ShowTree(int& nodeIndex, std::optional<int>& selectedIndex, Node*& pSelectedNode) const noexcept(!IS_DEBUG) {
	const int currentNodeIndex = nodeIndex;
	nodeIndex++;
	const int nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((currentNodeIndex == selectedIndex.value_or(-1)) ? ImGuiTreeNodeFlags_Selected : 0)
		| (childPtrs.empty() ? ImGuiTreeNodeFlags_Leaf : 0);
	// int to void* need to cast.
	if (ImGui::TreeNodeEx((void*)(intptr_t)currentNodeIndex, nodeFlags, name.c_str())) {
		if (ImGui::IsItemClicked()) {
			selectedIndex = currentNodeIndex;
			// future need to modify
			pSelectedNode = const_cast<Node*>(this);
		}
		for (const auto& pChild : childPtrs) {
			pChild->ShowTree(nodeIndex, selectedIndex, pSelectedNode);
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
		int nodeIndex = 0;
		if (ImGui::Begin(windowName)) {
			ImGui::Columns(2, nullptr, true);
			root.ShowTree(nodeIndex, selectedIndex, pSelectedNode);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr) {
				auto& transform = transforms[*selectedIndex];

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
		const auto& transform = transforms.at(*selectedIndex);
		return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) 
			* DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}
	Node* GetSelectedNode() const noexcept(!IS_DEBUG) {
		return pSelectedNode;
	}
private:
	std::optional<int> selectedIndex;
	Node* pSelectedNode = nullptr;
	struct TransformParameters{
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
	pWindow(std::make_unique<ModelWindow>())
{
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
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));
	}

	pRoot = ParseNode(*pScene->mRootNode);
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
std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh) {
	namespace dx = DirectX;
	using custom::VertexLayout;

	custom::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
	));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++) {
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
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

	std::vector<std::unique_ptr<Bindable>> bindablePtrs;

	bindablePtrs.push_back(std::make_unique<VertexBuffer>(gfx, vbuf));

	bindablePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));

	auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));

	bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

	bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

	struct PSMaterialConstant {
		DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3] = { 0.0f, 0.0f, 0.0f };
	} pmc;
	bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}
std::unique_ptr<Node> Model::ParseNode(const aiNode& node) {
	namespace dx = DirectX;
	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++) {
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++) {
		pNode->AddChild(ParseNode(*node.mChildren[i]));
	}

	return pNode;
}