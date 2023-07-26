#include "Window.h"
#include <sstream>
#include "resource.h"
#include "imgui/imgui_impl_win32.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hbrBackground = nullptr;
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wc.hCursor = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass() {
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept {
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept {
	return wndClass.hInst;
}


Window::Window(int width, int height, const char* name)
	:
	width(width),
	height(height)
{
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (!AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)) {
		throw WND_LAST_EXCEPT();
	}
	
	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);
	// check error
	if (hWnd == nullptr) {
		throw WND_LAST_EXCEPT();
	}

	ShowWindow(hWnd, SW_SHOWDEFAULT);

	// init imgui win32 impl
	ImGui_ImplWin32_Init(hWnd);

	DisableCursor();
	//EnableCursor();

	// create graphics object
	pGfx = std::make_unique<Graphics>(hWnd, width, height);

	// register mouse raw input device
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; // mouse page
	rid.usUsage = 0x02; // mouse usage
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE) {
		throw WND_LAST_EXCEPT();
	}
}

Window::~Window() {
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& title) {
	if (SetWindowText(hWnd, title.c_str()) == 0) {
		throw WND_LAST_EXCEPT();
	}
}

void Window::EnableCursor() noexcept {
	cursorEnabled = true;
	ShowCursor();
	EnableImGuiMouse();
	FreeCursor();
}

void Window::DisableCursor() noexcept {
	cursorEnabled = false;
	HideCursor();
	DisableImGuiMouse();
	ConfineCursor();
}

bool Window::CursorEnabled() const noexcept {
	return cursorEnabled;
}

std::optional<int> Window::ProcessMessage() {
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			return static_cast<int>(msg.wParam);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return {};
}

Graphics& Window::Gfx() {
	if (!pGfx) {
		throw WND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

void Window::ConfineCursor() noexcept {
	RECT rect;
	GetClientRect(hWnd, &rect);
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}

void Window::FreeCursor() noexcept {
	ClipCursor(nullptr);
}

void Window::ShowCursor() noexcept {
	while (::ShowCursor(TRUE) < 0);
}

void Window::HideCursor() noexcept {
	while (::ShowCursor(FALSE) >= 0);
}

void Window::EnableImGuiMouse() noexcept {
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void Window::DisableImGuiMouse() noexcept {
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	if (msg == WM_NCCREATE) {
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set pWnd data to window api side
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// change window procedure setup -> thunk
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
		return true;
	}
	const auto& imio = ImGui::GetIO();

	switch (msg) {
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		// destroy only deconstrutor is evoke
		return 0;
	}
	case WM_KILLFOCUS:
	{
		keyboard.ClearState();
		break;
	}
#pragma region KEYBOARD MESSAGE
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		if (imio.WantCaptureKeyboard) {
			break;
		}
		if (!(lParam & 0x40000000) || keyboard.AutorepeatIsEnabled()) {
			keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		if (imio.WantCaptureKeyboard) {
			break;
		}
		keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	}
	case WM_CHAR:
	{
		if (imio.WantCaptureKeyboard) {
			break;
		}
		keyboard.OnChar(static_cast<unsigned char>(wParam));
		break;
	}
#pragma endregion

#pragma region MOUSE MESSAGE
	case WM_MOUSEMOVE:
	{
		if (imio.WantCaptureMouse) {
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		// in client region -> log move, and log enter + capture mouse (if not previously in window)
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height) {
			mouse.OnMouseMove(pt.x, pt.y);
			// mouse is enter to client
			if (!mouse.IsInWindow()) {
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		// not in client -> log move / maintain capture if button down
		else {
			if (wParam & (MK_LBUTTON | MK_RBUTTON)) {
				mouse.OnMouseMove(pt.x, pt.y);
			}
			// button up -> release capture / log event for leaving
			else {
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (imio.WantCaptureMouse) {
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		if (imio.WantCaptureMouse) {
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		if (imio.WantCaptureMouse) {
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		// release mouse if outside of window
		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height) {
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}
	case WM_RBUTTONUP:
	{
		if (imio.WantCaptureMouse) {
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		// release mouse if outside of window
		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height) {
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		if (imio.WantCaptureMouse) {
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	case WM_ACTIVATE:
		if (!cursorEnabled) {
			if (wParam & (WA_ACTIVE | WA_CLICKACTIVE)) {
				ConfineCursor();
				HideCursor();
			}
			else {
				FreeCursor();
				ShowCursor();
			}
		}
#pragma endregion

#pragma region RAW MOUSE MESSAGE
	case WM_INPUT:
		if (!mouse.RawEnabled()) {
			break;
		}
		UINT size = 0;
		// first get size of input data
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam), 
			RID_INPUT,
			nullptr, 
			&size, 
			sizeof(RAWINPUTHEADER)
			) == -1) {
			// bail msg processing if error
			break;
		}
		// allocate buffer and read in data
		rawBuffer.resize(size);
		// read in the input data
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			rawBuffer.data(),
			&size,
			sizeof(RAWINPUTHEADER)
			) != size) {
			break;
		}

		auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE &&
			// not click
			(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0)) {
			mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
		}
	}
#pragma endregion
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< "[File]" << GetFile() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hr);
}

const char* Window::NoGfxException::GetType() const noexcept {
	return "Window Exception [No Grahpics]";
}