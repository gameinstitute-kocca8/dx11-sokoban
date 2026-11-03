// StepApp.h - 단계 예제가 공유하는 '도구 코드'
//
// 여기에는 수업의 핵심 개념이 없다. 창 만들기, 메시지 루프, 로그, 경로, CI 캡처처럼
// 매 단계 반복되는 주변 코드만 둔다. Direct3D 객체(Device, SwapChain, 버퍼, 셰이더)는
// 이 파일에 절대 넣지 않는다. 그것들은 각 단계의 main.cpp에서 학생이 직접 만든다.
//
// 명령행 옵션 (CI와 기대 화면 생성용)
//   --capture <파일.png>  WARP(소프트웨어 렌더러)로 한 프레임 그리고 PNG로 저장한 뒤 종료
//   --time <초>           캡처할 때의 고정 시간 (기본 1.0). 회전 같은 애니메이션을 재현 가능하게 한다.
//   --warp                창 모드에서도 GPU 대신 WARP 사용
//   --level <파일>        (08단계~) 불러올 레벨 파일. 실행 파일 기준 경로 (예: levels/level02.txt)
//   --moves <wasd...>     (10단계~) 시작하자마자 이 이동을 순서대로 적용. 캡처로 '밀기 후' 화면을 만들 때 쓴다
#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <string>

struct ID3D11DeviceContext;
struct ID3D11Resource;
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;

namespace step {

struct Options {
    bool capture = false;
    std::wstring capturePath;
    float captureTime = 1.0f;
    bool warp = false;
    std::wstring level;  // 비어 있으면 게임의 기본 레벨
    std::string moves;   // 지도 기준 방향 w(위) a(왼쪽) s(아래) d(오른쪽)
};

class StepApp {
public:
    StepApp(const wchar_t* title, UINT width = 1280, UINT height = 720);
    virtual ~StepApp() = default;
    StepApp(const StepApp&) = delete;
    StepApp& operator=(const StepApp&) = delete;

    // 창을 만들고 OnInit → (OnUpdate → OnRender → OnPresent) 반복. 프로세스 종료 코드를 돌려준다.
    int Run(HINSTANCE instance);

protected:
    virtual bool OnInit() = 0;                         // Direct3D 객체 생성. 실패하면 false
    virtual void OnUpdate(float deltaSeconds, float totalSeconds) {
        (void)deltaSeconds;
        (void)totalSeconds;
    }
    virtual void OnRender() = 0;                       // 백 버퍼에 그리기
    virtual void OnPresent() = 0;                      // 백 버퍼를 화면으로
    virtual bool OnCapture(const std::wstring& path) = 0;  // 보통 SaveTexturePng 한 줄
    virtual void OnWindowMessage(UINT message, WPARAM wParam, LPARAM lParam) {
        (void)message;
        (void)wParam;
        (void)lParam;
    }

    HWND Window() const { return window_; }
    UINT Width() const { return width_; }
    UINT Height() const { return height_; }
    const Options& GetOptions() const { return options_; }
    // 캡처 중이거나 --warp이면 true. Device를 만들 때 드라이버 종류를 고르는 데 쓴다.
    bool UseWarp() const { return options_.warp || options_.capture; }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    std::wstring title_;
    UINT width_;
    UINT height_;
    HWND window_ = nullptr;
    Options options_;
};

// printf 형식 로그. Visual Studio 출력 창과 콘솔(있으면) 모두에 찍는다.
void Log(const char* format, ...);

// HRESULT 검사. 실패하면 무엇이 실패했는지 로그를 남기고 false.
//   if (!step::Check(hr, "CreateBuffer(vertex)")) return false;
bool Check(long hr, const char* what);

// 실행 파일이 있는 폴더 기준의 경로. 셰이더나 텍스처 파일을 찾을 때 쓴다.
std::wstring ExePath(const wchar_t* relative);

// HLSL 파일을 컴파일한다. 실패하면 컴파일러 오류(파일, 줄 번호 포함)를 로그로 남긴다.
//   relativePath는 실행 파일 기준 경로 (예: L"shaders/Triangle.hlsl")
bool CompileShader(const wchar_t* relativePath, const char* entryPoint, const char* target, ID3DBlob** blob);

// 텍스처(보통 백 버퍼)를 PNG로 저장한다. CI 캡처용.
bool SaveTexturePng(ID3D11DeviceContext* context, ID3D11Resource* texture, const std::wstring& path);

}  // namespace step
