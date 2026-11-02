// StepApp.cpp - 도구 코드 구현. 수업 시간에 읽을 필요는 없지만, 숨긴 마법도 없다.
#include "StepApp.h"

#include <shellapi.h>  // CommandLineToArgvW

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wincodec.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "ScreenGrab.h"  // DirectXTK

namespace step {

namespace {

bool g_hasConsole = false;

Options ParseOptions() {
    Options options;
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    for (int i = 1; i < argc; ++i) {
        const std::wstring arg = argv[i];
        if (arg == L"--capture" && i + 1 < argc) {
            options.capture = true;
            options.capturePath = argv[++i];
        } else if (arg == L"--time" && i + 1 < argc) {
            options.captureTime = static_cast<float>(_wtof(argv[++i]));
        } else if (arg == L"--warp") {
            options.warp = true;
        }
    }
    LocalFree(argv);
    return options;
}

// 명령 프롬프트나 CI에서 실행했을 때 로그가 보이도록 부모 콘솔에 붙는다.
void AttachParentConsole() {
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE* stream = nullptr;
        freopen_s(&stream, "CONOUT$", "w", stdout);
        freopen_s(&stream, "CONOUT$", "w", stderr);
        SetConsoleOutputCP(CP_UTF8);
        g_hasConsole = true;
    }
}

}  // namespace

void Log(const char* format, ...) {
    char buffer[2048];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // 소스 파일이 UTF-8이므로 출력 창에는 UTF-16으로 바꿔서 보낸다.
    wchar_t wide[2048];
    if (MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wide, 2048) > 0) {
        OutputDebugStringW(wide);
        OutputDebugStringW(L"\n");
    }
    if (g_hasConsole) {
        std::fprintf(stderr, "%s\n", buffer);
        std::fflush(stderr);
    }
}

bool Check(long hr, const char* what) {
    if (SUCCEEDED(hr)) return true;
    Log("[실패] %s (HRESULT 0x%08lX)", what, static_cast<unsigned long>(hr));
    return false;
}

std::wstring ExePath(const wchar_t* relative) {
    wchar_t buffer[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    std::wstring path = buffer;
    const size_t slash = path.find_last_of(L"\\/");
    path = (slash == std::wstring::npos) ? L"" : path.substr(0, slash + 1);
    return path + relative;
}

bool CompileShader(const wchar_t* relativePath, const char* entryPoint, const char* target, ID3DBlob** blob) {
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    const std::wstring path = ExePath(relativePath);
    ID3DBlob* errors = nullptr;
    const HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint,
                                          target, flags, 0, blob, &errors);
    if (errors) {
        Log("[셰이더] %s", static_cast<const char*>(errors->GetBufferPointer()));
        errors->Release();
    }
    if (FAILED(hr)) {
        char narrow[MAX_PATH * 3] = {};
        WideCharToMultiByte(CP_UTF8, 0, path.c_str(), -1, narrow, sizeof(narrow), nullptr, nullptr);
        Log("[실패] 셰이더 컴파일: %s (%s, %s) HRESULT 0x%08lX", narrow, entryPoint, target,
            static_cast<unsigned long>(hr));
        return false;
    }
    return true;
}

bool SaveTexturePng(ID3D11DeviceContext* context, ID3D11Resource* texture, const std::wstring& path) {
    if (!context || !texture) {
        Log("[실패] 캡처: context 또는 texture가 없음 (아직 만들지 않았나?)");
        return false;
    }
    return Check(DirectX::SaveWICTextureToFile(context, texture, GUID_ContainerFormatPng, path.c_str()),
                 "SaveWICTextureToFile");
}

// ---------------------------------------------------------------------------

StepApp::StepApp(const wchar_t* title, UINT width, UINT height) : title_(title), width_(width), height_(height) {}

LRESULT CALLBACK StepApp::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* app = reinterpret_cast<StepApp*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (app) app->OnWindowMessage(message, wParam, lParam);

    switch (message) {
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, message, wParam, lParam);
    }
}

int StepApp::Run(HINSTANCE instance) {
    options_ = ParseOptions();
    if (options_.capture) AttachParentConsole();

    // WIC(PNG 저장, 텍스처 로드)에 필요
    const HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(comResult)) {
        Check(comResult, "CoInitializeEx");
        return 1;
    }

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"DX11StepWindow";
    RegisterClassExW(&wc);

    // 클라이언트 영역이 정확히 width x height가 되도록 창 크기를 계산
    RECT rect = {0, 0, static_cast<LONG>(width_), static_cast<LONG>(height_)};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    window_ = CreateWindowExW(0, wc.lpszClassName, title_.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                              CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr,
                              instance, nullptr);
    if (!window_) {
        Log("[실패] CreateWindowExW");
        return 1;
    }
    SetWindowLongPtrW(window_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    if (!OnInit()) {
        Log("OnInit 실패. 위의 [실패] 로그를 확인하세요.");
        if (!options_.capture) {
            MessageBoxW(window_, L"초기화 실패. Visual Studio 출력 창의 [실패] 로그를 확인하세요.", title_.c_str(),
                        MB_OK | MB_ICONERROR);
        }
        return 1;
    }

    if (options_.capture) {
        OnUpdate(0.0f, options_.captureTime);
        OnRender();
        const bool saved = OnCapture(options_.capturePath);
        Log(saved ? "캡처 저장 완료" : "캡처 실패");
        return saved ? 0 : 1;
    }

    ShowWindow(window_, SW_SHOW);

    LARGE_INTEGER frequency, start, previous, now;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    previous = start;

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            continue;
        }
        QueryPerformanceCounter(&now);
        const float delta = static_cast<float>(now.QuadPart - previous.QuadPart) / frequency.QuadPart;
        const float total = static_cast<float>(now.QuadPart - start.QuadPart) / frequency.QuadPart;
        previous = now;

        OnUpdate(delta, total);
        OnRender();
        OnPresent();
    }
    return static_cast<int>(msg.wParam);
}

}  // namespace step
