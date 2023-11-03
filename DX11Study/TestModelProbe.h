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
public:
	MP(std::string name)
		:
		name(std::move(name)) 
	{}
	void SpawnWindow(Model& model) {

		ImGui::Begin(std::string(name + " Anim").c_str());
		ImGui::SliderFloat("Animation Tick", &model.animationTick, 0.0f, 140.0f);
		ImGui::End();

		ImGui::Begin(std::string(name + " bone matrixes").c_str());
		const auto& boneMatrixes = model.GetBoneMatrixes();
		std::string name;
		for (int i = 0; i < boneMatrixes.size(); i++) {
			for (auto map : model.nameBoneIndexMap) {
				if (map.second == i) {
					name = map.first;
					break;
				}
			}
			const auto finalMatrix = boneMatrixes[i].GetFinalMatrix();
			ImGui::Text(name.c_str());
			ImGui::Text("%f %f %f %f", finalMatrix._11, finalMatrix._12, finalMatrix._13, finalMatrix._14);
			ImGui::Text("%f %f %f %f", finalMatrix._21, finalMatrix._22, finalMatrix._23, finalMatrix._24);
			ImGui::Text("%f %f %f %f", finalMatrix._31, finalMatrix._32, finalMatrix._33, finalMatrix._34);
			ImGui::Text("%f %f %f %f", finalMatrix._41, finalMatrix._42, finalMatrix._43, finalMatrix._44);
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
			dcheck(ImGui::SliderFloat("X", &tf.x, -60.f, 60.f));
			dcheck(ImGui::SliderFloat("Y", &tf.y, -60.f, 60.f));
			dcheck(ImGui::SliderFloat("Z", &tf.z, -60.f, 60.f));
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
			dcheck(ImGui::SliderAngle("X-rotation", &tf.xRot, -180.0f, 180.0f));
			dcheck(ImGui::SliderAngle("Y-rotation", &tf.yRot, -180.0f, 180.0f));
			dcheck(ImGui::SliderAngle("Z-rotation", &tf.zRot, -180.0f, 180.0f));
			if (dirty) {
				pSelectedNode->SetAppliedTransform(
					dx::XMMatrixRotationX(tf.xRot) *
					dx::XMMatrixRotationY(tf.yRot) *
					dx::XMMatrixRotationZ(tf.zRot) *
					dx::XMMatrixTranslation(tf.x, tf.y, tf.z)
				);
			}
			//const auto& ftf = ResolveFinalTransform();
			//ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Final Translation");
			//ImGui::Text("%f %f %f", tf.x, tf.y, tf.z);

			//ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Final Orientation");
			//ImGui::Text("%f %f %f", tf.xRot, tf.yRot, tf.zRot);
			const auto& transform = pSelectedNode->GetAppliedTransform();
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "applied Transform Matrix");
			ImGui::Text("%f %f %f %f", transform._11, transform._12, transform._13, transform._14);
			ImGui::Text("%f %f %f %f", transform._21, transform._22, transform._23, transform._24);
			ImGui::Text("%f %f %f %f", transform._31, transform._32, transform._33, transform._34);
			ImGui::Text("%f %f %f %f", transform._41, transform._42, transform._43, transform._44);
			const auto& tfp = GetTransformParameters(transform);
			ImGui::Text("Translate %f %f %f", tfp.x, tfp.y, tfp.z);
			ImGui::Text("Rotation %f %f %f", tfp.xRot, tfp.yRot, tfp.zRot);

			const auto& finalTransform = pSelectedNode->GetFinalTransform();
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Final Transform Matrix");
			ImGui::Text("%f %f %f %f", finalTransform._11, finalTransform._12, finalTransform._13, finalTransform._14);
			ImGui::Text("%f %f %f %f", finalTransform._21, finalTransform._22, finalTransform._23, finalTransform._24);
			ImGui::Text("%f %f %f %f", finalTransform._31, finalTransform._32, finalTransform._33, finalTransform._34);
			ImGui::Text("%f %f %f %f", finalTransform._41, finalTransform._42, finalTransform._43, finalTransform._44);
			const auto& ftfp = GetTransformParameters(finalTransform);
			ImGui::Text("Translate %f %f %f", ftfp.x, ftfp.y, ftfp.z);
			ImGui::Text("Rotation %f %f %f", ftfp.xRot, ftfp.yRot, ftfp.zRot);


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