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
	void SpawnWindow(Model& model) {
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
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Applied Matrix");
			auto diff = dx::XMMatrixTranspose(dx::XMMatrixRotationX(tf.xRot) *
				dx::XMMatrixRotationY(tf.yRot) *
				dx::XMMatrixRotationZ(tf.zRot) *
				dx::XMMatrixTranslation(tf.x, tf.y, tf.z));
			if (dirty) {
				pSelectedNode->SetAppliedTransform(diff);
			}
			for (auto meshPtr : pSelectedNode->meshPtrs) {
				ImGui::Text(meshPtr->name.c_str());
			}
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
};