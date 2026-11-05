// Renderer.h - Render.lib의 유일한 공개 클래스
//
// 이 헤더는 d3d11.h를 포함하지 않는다. Direct3D 객체는 모두 Renderer.cpp 안의 Impl에 숨긴다.
// 그래서 이 헤더를 쓰는 Engine과 게임 코드는 Direct3D를 모른 채 "큐브를 이 위치에 이 색으로"만 말한다.
//
// Render는 게임을 모른다: 벽, 상자, 목표 같은 단어가 이 폴더에 나오면 안 된다 (tools/check_layers.py).
#pragma once

#include <memory>
#include <string>

#include "SimpleMath.h"  // DirectXTK

namespace render {

using Matrix = DirectX::SimpleMath::Matrix;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;

// 메시 손잡이. Renderer 안의 메시 목록에서 몇 번째인지일 뿐이다.
struct MeshHandle {
    int index = -1;
    bool IsValid() const { return index >= 0; }
};

class Renderer {
public:
    Renderer();
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // window는 HWND. 헤더가 windows.h를 끌고 오지 않도록 void*로 받는다.
    bool Initialize(void* window, unsigned width, unsigned height, bool useWarp);

    // 한 변 1, 원점 중심, 면마다 법선이 있는 정육면체 (06단계의 그 큐브)
    MeshHandle CreateCube();

    // 11단계: 화면 글자. 글꼴 파일은 실행 파일 기준 경로 (예: L"fonts/ui.spritefont")
    bool LoadFont(const std::wstring& relativePath);

    // 한 프레임: BeginFrame → SetCamera/SetLight → DrawMesh 여러 번 → DrawScreenText 여러 번 → EndFrame → Present
    void BeginFrame(const Vector4& clearColor);
    void SetCamera(const Matrix& view, const Matrix& projection);
    void SetLight(const Vector3& direction, float ambient);
    void DrawMesh(MeshHandle mesh, const Matrix& world, const Vector4& color);
    // 이름이 DrawText가 아닌 이유: windows.h가 DrawText를 매크로(DrawTextW)로 정의해 이름이 바뀌어 버린다.
    // 글자는 바로 그리지 않고 모아 두었다가 EndFrame에서 3D 위에 한 번에 그린다. x, y는 왼쪽 위 기준 픽셀
    void DrawScreenText(const std::string& text, float x, float y, const Vector4& color, float scale = 1.0f);
    void EndFrame();
    void Present();

    bool SaveBackBufferPng(const std::wstring& path);  // CI 캡처용
    float AspectRatio() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace render
