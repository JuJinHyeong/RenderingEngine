#pragma once
#include "Window.h"
#include <string>

void TestDynamicMeshLoading();
void TestScaleMatrixTranslation();
void D3DTestScratchPad(Window& wnd);
void AssimpTest(const std::string& filename, float tick = 0.0f);
void JsonTest();
void SceneTest(Graphics& gfx);
void MatrixTest();
void CurlTest();