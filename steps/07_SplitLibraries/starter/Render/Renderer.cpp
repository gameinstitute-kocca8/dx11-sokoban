// Renderer.cpp - Direct3D 11을 아는 유일한 파일
//
// 07단계 과제: 06단계 main.cpp의 코드를 이 파일로 '옮긴다'. 새로 짜는 것이 아니라 자리를 바꾸는 것이다.
//   06 OnInit           → Initialize, CreateCube
//   06 OnRender 앞부분  → BeginFrame
//   06 DrawCube         → DrawMesh
//   06 OnPresent        → Present
#include "Render/Renderer.h"

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>
#include <vector>

#include "StepApp.h"  // step::Log, Check, CompileShader, SaveTexturePng (도구 코드)

using Microsoft::WRL::ComPtr;

namespace render {

namespace {

struct Vertex {
    float position[3];
    float normal[3];
};

// 정육면체: 면마다 법선이 다르므로 면마다 정점 4개씩, 총 24개 (04단계와 같은 위치, 같은 감김 순서)
const Vertex kCubeVertices[] = {
    // +X
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    // -X
    {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
    // +Y
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    // -Y
    {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
    // +Z
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    // -Z
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
};

const std::uint16_t kCubeIndices[] = {
    0,  1,  2,  0,  2,  3,   // +X
    4,  5,  6,  4,  6,  7,   // -X
    8,  9,  10, 8,  10, 11,  // +Y
    12, 13, 14, 12, 14, 15,  // -Y
    16, 17, 18, 16, 18, 19,  // +Z
    20, 21, 22, 20, 22, 23,  // -Z
};

// shaders/Lit.hlsl의 cbuffer PerObject와 같은 배치 (160바이트)
struct PerObjectConstants {
    Matrix worldViewProj;
    Matrix world;
    Vector4 color;
    Vector4 lightDirection;  // xyz: 방향, w: 주변광
};
static_assert(sizeof(PerObjectConstants) % 16 == 0, "Constant Buffer 크기는 16의 배수");

struct Mesh {
    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;
    UINT indexCount = 0;
};

}  // namespace

// 06단계 main.cpp의 멤버 변수들이 여기로 온다.
struct Renderer::Impl {
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<IDXGISwapChain> swapChain;
    ComPtr<ID3D11Texture2D> backBuffer;
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    ComPtr<ID3D11Texture2D> depthTexture;
    ComPtr<ID3D11DepthStencilView> depthStencilView;
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11InputLayout> inputLayout;
    ComPtr<ID3D11Buffer> constantBuffer;

    std::vector<Mesh> meshes;
    Matrix view;
    Matrix projection;
    Vector4 lightDirection = Vector4(0.0f, -1.0f, 0.0f, 0.25f);
    unsigned width = 0;
    unsigned height = 0;
};

Renderer::Renderer() : impl_(std::make_unique<Impl>()) {}
Renderer::~Renderer() = default;

bool Renderer::Initialize(void* window, unsigned width, unsigned height, bool useWarp) {
    Impl& r = *impl_;
    r.width = width;
    r.height = height;
    // TODO(07): 06단계 OnInit의 Device/SwapChain/RTV/깊이 버퍼/셰이더/Input Layout/Constant Buffer 생성을 옮긴다.
    //   힌트: Width() → width, Window() → static_cast<HWND>(window), UseWarp() → useWarp, device_ → r.device
    //   힌트: 정점/인덱스 버퍼는 여기가 아니라 CreateCube로 간다. 메시는 여러 개일 수 있기 때문이다.
    (void)window;
    (void)useWarp;
    return true;
}

MeshHandle Renderer::CreateCube() {
    Impl& r = *impl_;
    Mesh mesh;
    // TODO(07): 06단계 OnInit의 정점/인덱스 버퍼 생성을 옮겨 mesh에 채운다.
    //   힌트: 실패하면 빈 MeshHandle{}을 돌려준다. 성공하면 r.meshes에 넣고 그 번호를 돌려준다.
    (void)r;
    (void)mesh;
    return {};
}

void Renderer::BeginFrame(const Vector4& clearColor) {
    Impl& r = *impl_;
    // TODO(07): 06단계 OnRender의 '그리기 전 준비'를 옮긴다: 색/깊이 지우기, 렌더 타깃, 뷰포트, 셰이더, Input Layout.
    //   힌트: 메시마다 달라지는 정점/인덱스 버퍼는 DrawMesh에서 바인딩한다.
    (void)r;
    (void)clearColor;
}

void Renderer::SetCamera(const Matrix& view, const Matrix& projection) {
    impl_->view = view;
    impl_->projection = projection;
}

void Renderer::SetLight(const Vector3& direction, float ambient) {
    Vector3 d = direction;
    d.Normalize();
    impl_->lightDirection = Vector4(d.x, d.y, d.z, ambient);
}

void Renderer::DrawMesh(MeshHandle handle, const Matrix& world, const Vector4& color) {
    Impl& r = *impl_;
    // TODO(07): 06단계 DrawCube를 옮긴다. 이번에는 메시를 handle로 고르고, 정점/인덱스 버퍼도 여기서 바인딩한다.
    //   힌트: 잘못된 handle(IsValid가 false이거나 범위 밖)은 무시한다. 게임 코드의 실수로 렌더러가 죽으면 안 된다.
    (void)r;
    (void)handle;
    (void)world;
    (void)color;
}

void Renderer::Present() {
    // TODO(07): 06단계 OnPresent를 옮긴다.
}

bool Renderer::SaveBackBufferPng(const std::wstring& path) {
    return step::SaveTexturePng(impl_->context.Get(), impl_->backBuffer.Get(), path);
}

float Renderer::AspectRatio() const {
    return impl_->height == 0 ? 1.0f : static_cast<float>(impl_->width) / static_cast<float>(impl_->height);
}

}  // namespace render
