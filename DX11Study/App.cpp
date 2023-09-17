#include "DataMacros.h"
#include "App.h"
#include "CustomMath.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <dxtex/DirectXTex.h>
#include <vector>
#include "Testing.h"
#include "Material.h"
#include "ModelProbe.h"
#include "ExtendedXMMath.h"

int width = 1280;
int height = 720;

App::App()
	:
	wnd(width, height, "First App"),
	light(wnd.Gfx()) 
{
	//cube1.SetPos(DirectX::XMFLOAT3{20.0f, 0.0f, 0.0f});
	// TODO command line scripts
	//bluePlane.SetPos(cam.GetPos());
	//redPlane.SetPos(cam.GetPos());

	//{
	//	std::string path = "models/gobber/GoblinX.obj";
	//	Assimp::Importer imp;
	//	const auto pScene = imp.ReadFile(path,
	//		aiProcess_Triangulate |
	//		aiProcess_JoinIdenticalVertices |
	//		aiProcess_ConvertToLeftHanded |
	//		aiProcess_GenNormals |
	//		aiProcess_CalcTangentSpace
	//	);
	//	Material mat{ wnd.Gfx(),*pScene->mMaterials[1], path };
	//	pLoaded = std::make_unique<Mesh>(wnd.Gfx(), mat, *pScene->mMeshes[0]);
	//}

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, static_cast<float>(height) / static_cast<float>(width), 0.5f, 400.0f));
}


int App::Go() {
	while (true) {
		if (const auto ecode = Window::ProcessMessage()) {
			return *ecode;
		}
		DoFrame();
	}

	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		DoFrame();
	}
	if (gResult == -1) {
		throw WND_LAST_EXCEPT();
	}

	return (int)msg.wParam;
}

void App::DoFrame() {
	const auto dt = timer.Mark() * speed_factor;

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());

	//nano.Draw(wnd.Gfx());
	//gobber.Draw(wnd.Gfx());
	//wall.Draw(wnd.Gfx());
	//light.Draw(wnd.Gfx());
	//sponza.Draw(wnd.Gfx());
	//bluePlane.Draw(wnd.Gfx());
	//redPlane.Draw(wnd.Gfx());
	//cube1.Draw(wnd.Gfx());
	//cube2.Draw(wnd.Gfx());
	//cube1.DrawOutLine(wnd.Gfx());
	//cube2.DrawOutLine(wnd.Gfx());

	light.Submit(fc);
	cube1.Submit(fc);
	sponza.Submit(fc);
	cube2.Submit(fc);
	//wall.Submit(fc);
	//gobber.Submit(fc);
	//nano.Submit(fc);
	//pLoaded->Submit(fc, DirectX::XMMatrixIdentity());
	fc.Execute(wnd.Gfx());

	while (const auto e = wnd.keyboard.ReadKey()) {
		if (!e->IsPress()) {
			continue;
		}
		switch (e->GetCode()) {
		case VK_ESCAPE:
			if (wnd.CursorEnabled()) {
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else {
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_F1:
			showDemoWindow = !showDemoWindow;
			break;
		default:
			break;
		}
	}

	if (!wnd.CursorEnabled()) {
		if (wnd.keyboard.KeyIsPressed('W')) {
			cam.Translate({ 0.0f, 0.0f, dt });
		}
		if (wnd.keyboard.KeyIsPressed('A')) {
			cam.Translate({ -dt, 0.0f, 0.0f });
		}
		if (wnd.keyboard.KeyIsPressed('S')) {
			cam.Translate({ 0.0f, 0.0f, -dt });
		}
		if (wnd.keyboard.KeyIsPressed('D')) {
			cam.Translate({ dt, 0.0f, 0.0f });
		}
		if (wnd.keyboard.KeyIsPressed('R')) {
			cam.Translate({ 0.0f, dt, 0.0f });
		}
		if (wnd.keyboard.KeyIsPressed('F')) {
			cam.Translate({ 0.0f, -dt, 0.0f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta()) {
		if (!wnd.CursorEnabled()) {
			cam.Rotate(static_cast<float>(delta->x), static_cast<float>(delta->y));
		}
	}

	class TP : public TechniqueProbe {
	public:
		void OnSetTechnique() override {
			using namespace std::string_literals;
			ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, pTech->GetName().c_str());
			bool active = pTech->IsActive();
			ImGui::Checkbox(("Tech Active##"s + std::to_string(techIdx)).c_str(), &active);
			pTech->SetActiveState(active);
		}
		bool OnVisitBuffer(Dcb::Buffer& buf) override {
			float dirty = false;
			const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
			auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufIdx)]
			(const char* label) mutable {
				tagScratch = label + tagString;
				return tagScratch.c_str();
			};

			if (auto v = buf["scale"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f));
			}
			if (auto v = buf["offset"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("offset"), &v, 0.01f, 0.5f));
			}
			if (auto v = buf["materialColor"]; v.Exists()) {
				dcheck(ImGui::ColorPicker3(tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
			}
			if (auto v = buf["specularColor"]; v.Exists()) {
				dcheck(ImGui::ColorPicker3(tag("Spec. Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
			}
			if (auto v = buf["specularGloss"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f));
			}
			if (auto v = buf["specularWeight"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Spec. Weight"), &v, 0.0f, 2.0f));
			}
			if (auto v = buf["useNormalMap"]; v.Exists()) {
				dcheck(ImGui::Checkbox(tag("Normal Map Enable"), &v));
			}
			if (auto v = buf["useSpecularMap"]; v.Exists()) {
				dcheck(ImGui::Checkbox(tag("Specular Map Enable"), &v));
			}
			if (auto v = buf["normalMapWeight"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Normal Map Weight"), &v, 0.0f, 2.0f));
			}
			return dirty;
		}
	};

	class MP : public ModelProbe {
	public:void SpawnWindow(Model& model, std::string name)
	{
		ImGui::Begin(name.c_str());
		ImGui::Columns(2, nullptr, true);
		model.Accept(*this);

		ImGui::NextColumn();
		if (pSelectedNode != nullptr)
		{
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
			if (dirty)
			{
				pSelectedNode->SetAppliedTransform(
					DirectX::XMMatrixRotationX(tf.xRot) *
					DirectX::XMMatrixRotationY(tf.yRot) *
					DirectX::XMMatrixRotationZ(tf.zRot) *
					DirectX::XMMatrixTranslation(tf.x, tf.y, tf.z)
				);
			}
		}
		if (pSelectedNode != nullptr)
		{
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

			const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)node.GetId(), node_flags, node.GetName().c_str());
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
		std::unordered_map<int, TransformParameters> transformParams;
	private:
		TransformParameters& ResolveTransform() noexcept {
			const auto id = pSelectedNode->GetId();
			auto i = transformParams.find(id);
			if (i == transformParams.end()) {
				return LoadTransform(id);
			}
			return i->second;
		}
		TransformParameters& LoadTransform(int id) noexcept {
			const auto& applied = pSelectedNode->GetAppliedTransform();
			const auto angles = ExtractEulerAngles(applied);
			const auto translation = ExtractTranslation(applied);
			TransformParameters tp;
			tp.xRot = angles.x;
			tp.yRot = angles.y;
			tp.zRot = angles.z;
			tp.x = translation.x;
			tp.y = translation.y;
			tp.z = translation.z;
			return transformParams.insert({ id, {tp} }).first->second;
		}
	};
	static MP modelProbe;

	//pLoaded->Accept(probe);
	
	if (showDemoWindow) {
		cam.SpawnControlWindow();
		light.SpawnControlWindow();
		ShowModelDemoWindow();
		modelProbe.SpawnWindow(sponza, "sponza");
		//modelProbe.SpawnWindow(gobber, "gobber");
		//modelProbe.SpawnWindow(nano, "nano");
		//nanoProbe.SpawnWindow(nano, "nano");
		//gobberProbe.SpawnWindow(gobber, "gobber");
		cube1.SpawnControlWindow(wnd.Gfx(), "cube1");
		cube2.SpawnControlWindow(wnd.Gfx(), "cube2");
	}

	wnd.Gfx().EndFrame();
	fc.Reset();
}

void App::ShowModelDemoWindow() {
	//nano.ShowWindow("Nano");
	//gobber.ShowWindow("Goblin");
	//wall.ShowWindow("Wall");
	//bluePlane.SpawnControlWindow(wnd.Gfx(), "blue plane");
	//redPlane.SpawnControlWindow(wnd.Gfx(), "red plane");
	//sponza.ShowWindow("Sponza");
	//cube1.SpawnControlWindow(wnd.Gfx(), "cube1");
	//cube2.SpawnControlWindow(wnd.Gfx(), "cube2");
}

void App::ShowFrameRateWindow() {
	if (ImGui::Begin("Frame Rate")) {
		ImGui::Text("%d fps", static_cast<int>(1.0f / timer.Mark()));
	}
	ImGui::End();
}
