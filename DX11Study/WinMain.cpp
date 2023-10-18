#include "App.h"
#include "WindowsMessageMap.h"
#include "Mouse.h"
#include "Testing.h"

#include <sstream>

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow) {
	try {
		AllocConsole();
		FILE* stream;
		errno_t err = freopen_s(&stream, "CONOUT$", "wt", stdout);
		if (err != 0) {
			throw std::exception("Failed to redirect stdout to console");
		}
		AssimpTest("models/boblampclean/boblampclean.md5mesh");
		App app;
		return app.Go();
	}
	catch (const BasicException& e) {
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e) {
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...) {
		MessageBox(nullptr, "No details available", "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}