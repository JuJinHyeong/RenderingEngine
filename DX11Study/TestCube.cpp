#include "TestCube.h"
#include "Cube.h"
#include "BindableCommon.h"
#include "TransformDoubleCbuf.h"
#include "imgui/imgui.h"
#include "Stencil.h"
#include "DynamicConstant.h"

TestCube::TestCube(Graphics& gfx, float size) {
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cube::MakeIndependentTextured();
	model.Transform(dx::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();
	const auto geometryTag = "$cube." + std::to_string(size);
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique shade("Shade");
		{
			Step only(0);
			only.AddBindable(Texture::Resolve(gfx, "Images/brickwall.jpg"));
			only.AddBindable(Sampler::Resolve(gfx));
			auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
			auto pvsbc = pvs->GetBytecode();
			only.AddBindable(std::move(pvs));
			only.AddBindable(PixelShader::Resolve(gfx, "PhongPS.cso"));

			// dynamic constant buffer
			Dcb::RawLayout lay;
			lay.Add<Dcb::Float>("specularIntensity");
			lay.Add<Dcb::Float>("specularPower");
			auto buf = Dcb::Buffer(std::move(lay));
			buf["specularIntensity"] = 0.1f;
			buf["specularPower"] = 20.0f;
			only.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));

			only.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
			only.AddBindable(std::make_shared<TransformDoubleCbuf>(gfx, *this, 0u, 2u));
			shade.AddStep(std::move(only));
		}
		AddTechnique(std::move(shade));
	}
	{
		Technique outline("Outline");
		{
			Step mask(1);
			auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
			auto pvsbc = pvs->GetBytecode();
			mask.AddBindable(std::move(pvs));
			mask.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
			mask.AddBindable(std::make_shared<TransformCbuf>(gfx));

			outline.AddStep(std::move(mask));
		}
		{
			Step draw(2);
			auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
			auto pvsbc = pvs->GetBytecode();
			draw.AddBindable(std::move(pvs));
			draw.AddBindable(PixelShader::Resolve(gfx, "SolidPS.cso"));
			draw.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

			Dcb::RawLayout lay;
			lay.Add<Dcb::Float4>("color");
			auto buf = Dcb::Buffer(std::move(lay));
			buf["color"] = DirectX::XMFLOAT4{ 1.0f, 0.4f, 0.4f, 1.0f };
			draw.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));

			class TransformCbufScaling : public TransformCbuf {
			public:
				TransformCbufScaling(Graphics& gfx, float scale = 1.04f) 
					:
					TransformCbuf(gfx),
					buf(MakeLayout())
				{
					buf["scale"] = scale;
				}
				void Accept(TechniqueProbe& probe) override {
					probe.VisitBuffer(buf);
				}
				void Bind(Graphics& gfx) noexcept override {
					const float scale = buf["scale"];
					const auto scaleMatrix = DirectX::XMMatrixScaling(scale, scale, scale);
					auto xf = GetTransforms(gfx);
					xf.modelView = xf.modelView * scaleMatrix;
					xf.modelTransform = xf.modelTransform * scaleMatrix;
					UpdateBindImpl(gfx, xf);
				}
			private:
				static Dcb::RawLayout MakeLayout() {
					Dcb::RawLayout layout;
					layout.Add<Dcb::Float>("scale");
					return layout;
				}
			private:
				Dcb::Buffer buf;
			};
			draw.AddBindable(std::make_shared<TransformCbufScaling>(gfx));

			outline.AddStep(std::move(draw));
		}
		AddTechnique(std::move(outline));
	}

}

void TestCube::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	this->pos = pos;
}

void TestCube::SetRotation(float roll, float pitch, float yaw) noexcept {
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestCube::GetTransformXM() const noexcept {
	return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void TestCube::SpawnControlWindow(Graphics& gfx, const char* name) noexcept {
	if (ImGui::Begin(name)) {
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);

		class Probe : public TechniqueProbe {
			void OnSetTechnique() override {
				using namespace std::string_literals;
				ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, pTech->GetName().c_str());
				bool active = pTech->IsActive();
				ImGui::Checkbox(("Tech Active##"s + std::to_string(techIdx)).c_str(), &active);
				pTech->SetActiveState(active);
			}
			bool OnVisitBuffer(Dcb::Buffer& buf) override {
				float dirty = false;
				const auto dCheck = [&dirty](bool changed) {dirty = dirty || changed; };
				auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufIdx)]
				(const char* label) mutable {
					tagScratch = label + tagString;
					return tagScratch.c_str();
				};
				if (auto v = buf["scale"]; v.Exists()) {
					dCheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f"));
				}
				if (auto v = buf["color"]; v.Exists()) {
					//TODO: why?? doing like this??
					dCheck(ImGui::ColorPicker4(tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT4&>(v))));
				}
				if (auto v = buf["specularIntensity"]; v.Exists()) {
					dCheck(ImGui::SliderFloat(tag("Specular Intensity"), &v, 0.0f, 1.0f));
				}
				if (auto v = buf["specularPower"]; v.Exists()) {
					dCheck(ImGui::SliderFloat(tag("Glossiness"), &v, 0.0f, 100.0f, "%.1f"));
				}
				return dirty;
			}
		} probe;

		Accept(probe);
	}
	ImGui::End();
}