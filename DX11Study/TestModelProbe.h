#pragma once
#include "TechniqueProbe.h"
#include "ModelProbe.h"
#include "imgui/imgui.h"
#include "Technique.h"
#include "DynamicConstant.h"
#include "Model.h"
#include "Node.h"
#include <DirectXMath.h>
#include <string>
#include <unordered_map>
#include "ExtendedXMMath.h"

namespace dx = DirectX;

// Mesh techniques window
class TP : public TechniqueProbe {
public:
	void OnSetTechnique() override {
		using namespace std::string_literals;
		ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, pTech->GetName().c_str());
		bool active = pTech->IsActive();
		ImGui::Checkbox(("Tech Active##"s + std::to_string(techIdx)).c_str(), &active);
		pTech->SetActiveState(active);
	}
	bool OnVisitBuffer(Dcb::Buffer& buf) override {
		namespace dx = DirectX;
		float dirty = false;
		const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
		auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufIdx)]
		(const char* label) mutable {
			tagScratch = label + tagString;
			return tagScratch.c_str();
		};

		if (auto v = buf["selectedBoneIndex"]; v.Exists()) {
			dcheck(ImGui::SliderInt(tag("Selected Bone Index"), &v, 0, 32));
		}
		if (auto v = buf["scale"]; v.Exists()) {
			dcheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f"));
		}
		if (auto v = buf["offset"]; v.Exists()) {
			dcheck(ImGui::SliderFloat(tag("offset"), &v, 0.0f, 1.0f, "%.3f"));
		}
		if (auto v = buf["materialColor"]; v.Exists()) {
			dcheck(ImGui::ColorPicker3(tag("Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
		}
		if (auto v = buf["specularColor"]; v.Exists()) {
			dcheck(ImGui::ColorPicker3(tag("Spec. Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
		}
		if (auto v = buf["specularGloss"]; v.Exists()) {
			dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f"));
		}
		if (auto v = buf["specularWeight"]; v.Exists()) {
			dcheck(ImGui::SliderFloat(tag("Spec. Weight"), &v, 0.0f, 2.0f));
		}
		if (auto v = buf["useSpecularMap"]; v.Exists()) {
			dcheck(ImGui::Checkbox(tag("Spec. Map Enable"), &v));
		}
		if (auto v = buf["useNormalMap"]; v.Exists()) {
			dcheck(ImGui::Checkbox(tag("Normal Map Enable"), &v));
		}
		if (auto v = buf["normalMapWeight"]; v.Exists()) {
			dcheck(ImGui::SliderFloat(tag("Normal Map Weight"), &v, 0.0f, 2.0f));
		}
		return dirty;
	}
};

class MP : ModelProbe {
	friend class Model;
	friend class Node;
	friend class Mesh;
public:
	MP(std::string name)
		:
		name(std::move(name)) 
	{}
	void SpawnWindow(Model& model , Graphics& gfx) {
		ImGui::Begin(std::string(name + " Anim").c_str());
		ImGui::SliderFloat("Animation Tick", &model.animationTick, 0.0f, 1000.0f);
		ImGui::End();

		ImGui::Begin("view proj transform");
		DirectX::XMFLOAT4X4 float4x4;
		DirectX::XMStoreFloat4x4(&float4x4, gfx.GetCamera());
		ImGui::Text("View");
		ImGui::Text("%f %f %f %f", float4x4._11, float4x4._12, float4x4._13, float4x4._14);
		ImGui::Text("%f %f %f %f", float4x4._21, float4x4._22, float4x4._23, float4x4._24);
		ImGui::Text("%f %f %f %f", float4x4._31, float4x4._32, float4x4._33, float4x4._34);
		ImGui::Text("%f %f %f %f", float4x4._41, float4x4._42, float4x4._43, float4x4._44);
		DirectX::XMStoreFloat4x4(&float4x4, gfx.GetProjection());
		ImGui::Text("Proj");
		ImGui::Text("%f %f %f %f", float4x4._11, float4x4._12, float4x4._13, float4x4._14);
		ImGui::Text("%f %f %f %f", float4x4._21, float4x4._22, float4x4._23, float4x4._24);
		ImGui::Text("%f %f %f %f", float4x4._31, float4x4._32, float4x4._33, float4x4._34);
		ImGui::Text("%f %f %f %f", float4x4._41, float4x4._42, float4x4._43, float4x4._44);
		DirectX::XMStoreFloat4x4(&float4x4, gfx.GetCamera() * gfx.GetProjection());
		ImGui::Text("view proj");
		ImGui::Text("%f %f %f %f", float4x4._11, float4x4._12, float4x4._13, float4x4._14);
		ImGui::Text("%f %f %f %f", float4x4._21, float4x4._22, float4x4._23, float4x4._24);
		ImGui::Text("%f %f %f %f", float4x4._31, float4x4._32, float4x4._33, float4x4._34);
		ImGui::Text("%f %f %f %f", float4x4._41, float4x4._42, float4x4._43, float4x4._44);
		ImGui::End();

		ImGui::Begin(std::string(name + " bone matrixes").c_str());
		const auto& boneMatrixes = model.GetBoneMatrixes();
		std::string boneName;
		for (int i = 0; i < boneMatrixes.size(); i++) {
			for (auto map : model.nameBoneIndexMap) {
				if (map.second == i) {
					boneName = map.first;
					break;
				}
			}
			const auto finalMatrix = boneMatrixes[i].GetFinalMatrix();
			ImGui::Text(boneName.c_str());
			ImGui::Text("%f %f %f %f", finalMatrix._11, finalMatrix._12, finalMatrix._13, finalMatrix._14);
			ImGui::Text("%f %f %f %f", finalMatrix._21, finalMatrix._22, finalMatrix._23, finalMatrix._24);
			ImGui::Text("%f %f %f %f", finalMatrix._31, finalMatrix._32, finalMatrix._33, finalMatrix._34);
			ImGui::Text("%f %f %f %f", finalMatrix._41, finalMatrix._42, finalMatrix._43, finalMatrix._44);
		}
		ImGui::End();

		ImGui::Begin(std::string(name + " cpu calcualated vertices").c_str());
		
		ImGui::SliderInt("mesh index", &meshIndex, 0, model.meshPtrs.size() - 1, "%d");
		const auto& testMesh = model.meshPtrs[meshIndex];
		ImGui::Text("mesh name: %s", testMesh->name.c_str());
		for (int i = 0; i < testMesh->vertices.size(); i++) {
			const auto& boneIndex = testMesh->GetBoneIndex()[i];
			const auto& boneWeight = testMesh->GetBoneWeight()[i];
			auto aiVertex = testMesh->vertices[i];
			DirectX::XMFLOAT4 vertex = DirectX::XMFLOAT4(aiVertex.x, aiVertex.y, aiVertex.z, 1.0f);
			DirectX::XMMATRIX boneTransform = {};
			for (int j = 0; j < 4; j++) {
				boneTransform += boneMatrixes[boneIndex[j]].GetFinalMatrixXM() * boneWeight[j];
			}
			auto calcuatedVector = DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&vertex), boneTransform);
			DirectX::XMFLOAT4 bonePos;
			DirectX::XMStoreFloat4(&bonePos, calcuatedVector);
			ImGui::Text("%f %f %f %f", bonePos.x / bonePos.w, bonePos.y / bonePos.w, bonePos.z / bonePos.w, 1.0f);
		}
		ImGui::End();

		ImGui::Begin(name.c_str());
		ImGui::Columns(2, nullptr, true);
		model.Accept(*this);

		ImGui::NextColumn();
		if (pSelectedNode != nullptr) {
			bool dirty = false;
			const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
			auto& tf = ResolveTransform();
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Translation");
			dcheck(ImGui::SliderFloat("X", &tf.x, -0.3f, 0.3f));
			dcheck(ImGui::SliderFloat("Y", &tf.y, -0.3f, 0.3f));
			dcheck(ImGui::SliderFloat("Z", &tf.z, -0.3f, 0.3f));
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
			dcheck(ImGui::SliderFloat("X-rotation", &tf.xRot, -PI, PI));
			dcheck(ImGui::SliderFloat("Y-rotation", &tf.yRot, -PI, PI));
			dcheck(ImGui::SliderFloat("Z-rotation", &tf.zRot, -PI, PI));
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Matrix");
			auto diff = dx::XMMatrixTranspose(dx::XMMatrixRotationX(tf.xRot) *
				dx::XMMatrixRotationY(tf.yRot) *
				dx::XMMatrixRotationZ(tf.zRot) *
				dx::XMMatrixTranslation(tf.x, tf.y, tf.z));
			DirectX::XMFLOAT4X4 appliedTransform;
			dx::XMStoreFloat4x4(&appliedTransform, diff);
			ImGui::Text("%f %f %f %f", appliedTransform._11, appliedTransform._12, appliedTransform._13, appliedTransform._14);
			ImGui::Text("%f %f %f %f", appliedTransform._21, appliedTransform._22, appliedTransform._23, appliedTransform._24);
			ImGui::Text("%f %f %f %f", appliedTransform._31, appliedTransform._32, appliedTransform._33, appliedTransform._34);
			ImGui::Text("%f %f %f %f", appliedTransform._41, appliedTransform._42, appliedTransform._43, appliedTransform._44);
			if (dirty) {
				pSelectedNode->SetAppliedTransform(diff);
			}

			for (auto meshPtr : pSelectedNode->meshPtrs) {
				ImGui::Text(meshPtr->name.c_str());
			}

			//const auto& ftf = ResolveFinalTransform();
			//ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Final Translation");
			//ImGui::Text("%f %f %f", tf.x, tf.y, tf.z);

			//ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Final Orientation");
			//ImGui::Text("%f %f %f", tf.xRot, tf.yRot, tf.zRot);
			const auto& transform = pSelectedNode->GetTransform();
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Transform Matrix");
			ImGui::Text("%f %f %f %f", transform._11, transform._12, transform._13, transform._14);
			ImGui::Text("%f %f %f %f", transform._21, transform._22, transform._23, transform._24);
			ImGui::Text("%f %f %f %f", transform._31, transform._32, transform._33, transform._34);
			ImGui::Text("%f %f %f %f", transform._41, transform._42, transform._43, transform._44);
			const auto& tfp = GetTransformParameters(transform);
			ImGui::Text("Translate %f %f %f", tfp.x, tfp.y, tfp.z);
			ImGui::Text("Rotation %f %f %f", tfp.xRot * 180 / PI, tfp.yRot * 180 / PI, tfp.zRot * 180 / PI);

			const auto& animTransform = pSelectedNode->animatedTransform;
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Animated Transform Matrix");
			ImGui::Text("%f %f %f %f", animTransform._11, animTransform._12, animTransform._13, animTransform._14);
			ImGui::Text("%f %f %f %f", animTransform._21, animTransform._22, animTransform._23, animTransform._24);
			ImGui::Text("%f %f %f %f", animTransform._31, animTransform._32, animTransform._33, animTransform._34);
			ImGui::Text("%f %f %f %f", animTransform._41, animTransform._42, animTransform._43, animTransform._44);
			const auto& atfp = GetTransformParameters(animTransform);
			ImGui::Text("Translate %f %f %f", atfp.x, atfp.y, atfp.z);
			ImGui::Text("Rotation %f %f %f", atfp.xRot * 180 / PI, atfp.yRot * 180 / PI, atfp.zRot * 180 / PI);

			const auto& finalTransform = pSelectedNode->GetFinalTransform();
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Final Transform Matrix");
			ImGui::Text("%f %f %f %f", finalTransform._11, finalTransform._12, finalTransform._13, finalTransform._14);
			ImGui::Text("%f %f %f %f", finalTransform._21, finalTransform._22, finalTransform._23, finalTransform._24);
			ImGui::Text("%f %f %f %f", finalTransform._31, finalTransform._32, finalTransform._33, finalTransform._34);
			ImGui::Text("%f %f %f %f", finalTransform._41, finalTransform._42, finalTransform._43, finalTransform._44);
			const auto& ftfp = GetTransformParameters(finalTransform);
			ImGui::Text("Translate %f %f %f", ftfp.x, ftfp.y, ftfp.z);
			ImGui::Text("Rotation %f %f %f", ftfp.xRot * 180 / PI, ftfp.yRot * 180 / PI, ftfp.zRot * 180 / PI);


			TP probe;
			pSelectedNode->Accept(probe);
		}
		ImGui::End();
	}
protected:
	bool PushNode(Node& node) override {
		const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
		const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
			| ((node.GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
			| (node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf);
		const auto expanded = ImGui::TreeNodeEx(
			(void*)(intptr_t)node.GetId(),
			node_flags, node.GetName().c_str()
		);
		if (ImGui::IsItemClicked()) {
			struct Probe : public TechniqueProbe {
				virtual void OnSetTechnique() {
					if (pTech->GetName() == "Outline") {
						pTech->SetActiveState(highlighted);
					}
				}
				bool OnVisitBuffer(Dcb::Buffer& buf) override {
					return false;
				};
				bool highlighted = false;
			} probe;

			// remove highlight on prev-seleted node
			if (pSelectedNode != nullptr) {
				pSelectedNode->Accept(probe);
			}
			// add highlight on current selected node
			probe.highlighted = true;
			node.Accept(probe);
			pSelectedNode = &node;
		}
		return expanded;
	}
	void PopNode(Node& node) override {
		ImGui::TreePop();
	}
private:
	Node* pSelectedNode = nullptr;
	int meshIndex = 0;
	struct TransformParameters {
		float xRot = 0.0f;
		float yRot = 0.0f;
		float zRot = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> finalTransformParams;
	std::unordered_map<int, TransformParameters> transformParams;
	std::string name;
private:
	TransformParameters& ResolveTransform() noexcept {
		const auto id = pSelectedNode->GetId();
		auto i = transformParams.find(id);
		if (i == transformParams.end()) {
			return LoadTransform(id);
		}
		return i->second;
	}
	TransformParameters& ResolveFinalTransform() noexcept {
		const auto id = pSelectedNode->GetId();
		auto i = finalTransformParams.find(id);
		if (i == finalTransformParams.end()) {
			return LoadFinalTransform(id);
		}
		return i->second;
	}
	TransformParameters GetTransformParameters(const DirectX::XMFLOAT4X4& mat) noexcept {
		const auto angles = ExtractEulerAngles(mat);
		const auto translation = ExtractTranslation(mat);
		TransformParameters tp;
		tp.zRot = angles.z;
		tp.xRot = angles.x;
		tp.yRot = angles.y;
		tp.x = translation.x;
		tp.y = translation.y;
		tp.z = translation.z;
		return tp;
		
	}
	TransformParameters& LoadTransform(int id) noexcept {
		const auto& applied = pSelectedNode->GetAppliedTransform();
		const auto angles = ExtractEulerAngles(applied);
		const auto translation = ExtractTranslation(applied);
		TransformParameters tp;
		tp.zRot = angles.z;
		tp.xRot = angles.x;
		tp.yRot = angles.y;
		tp.x = translation.x;
		tp.y = translation.y;
		tp.z = translation.z;
		return transformParams.insert({ id,{ tp } }).first->second;
	}
	TransformParameters& LoadFinalTransform(int id) noexcept {
		const auto& finalT = pSelectedNode->GetTransform();
		const auto angles = ExtractEulerAngles(finalT);
		const auto translation = ExtractTranslation(finalT);
		TransformParameters tp;
		tp.zRot = angles.z;
		tp.xRot = angles.x;
		tp.yRot = angles.y;
		tp.x = translation.x;
		tp.y = translation.y;
		tp.z = translation.z;
		return transformParams.insert({ id,{ tp } }).first->second;
	}
};