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
    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferCount = 1;
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.OutputWindow = static_cast<HWND>(window);
    desc.SampleDesc.Count = 1;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    const D3D_DRIVER_TYPE driver = useWarp ? D3D_DRIVER_TYPE_WARP : D3D_DRIVER_TYPE_HARDWARE;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, driver, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, &desc,
                                               &r.swapChain, &r.device, nullptr, &r.context);
    if (FAILED(hr) && (flags & D3D11_CREATE_DEVICE_DEBUG)) {
        step::Log("디버그 레이어 없이 다시 시도합니다. 경고 메시지를 보려면 '그래픽 도구'를 설치하세요.");
        flags &= ~D3D11_CREATE_DEVICE_DEBUG;
        hr = D3D11CreateDeviceAndSwapChain(nullptr, driver, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, &desc,
                                           &r.swapChain, &r.device, nullptr, &r.context);
    }
    if (!step::Check(hr, "D3D11CreateDeviceAndSwapChain")) return false;
    if (!step::Check(r.swapChain->GetBuffer(0, IID_PPV_ARGS(&r.backBuffer)), "GetBuffer")) return false;
    if (!step::Check(r.device->CreateRenderTargetView(r.backBuffer.Get(), nullptr, &r.renderTargetView),
                     "CreateRenderTargetView")) {
        return false;
    }

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    if (!step::Check(r.device->CreateTexture2D(&depthDesc, nullptr, &r.depthTexture), "CreateTexture2D(depth)")) {
        return false;
    }
    if (!step::Check(r.device->CreateDepthStencilView(r.depthTexture.Get(), nullptr, &r.depthStencilView),
                     "CreateDepthStencilView")) {
        return false;
    }

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(PerObjectConstants);
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    if (!step::Check(r.device->CreateBuffer(&cbDesc, nullptr, &r.constantBuffer), "CreateBuffer(constant)")) {
        return false;
    }

    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;
    if (!step::CompileShader(L"shaders/Lit.hlsl", "VSMain", "vs_5_0", &vsBlob)) return false;
    if (!step::CompileShader(L"shaders/Lit.hlsl", "PSMain", "ps_5_0", &psBlob)) return false;
    if (!step::Check(r.device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr,
                                                  &r.vertexShader),
                     "CreateVertexShader")) {
        return false;
    }
    if (!step::Check(r.device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr,
                                                 &r.pixelShader),
                     "CreatePixelShader")) {
        return false;
    }
    const D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    if (!step::Check(r.device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(),
                                                 vsBlob->GetBufferSize(), &r.inputLayout),
                     "CreateInputLayout")) {
        return false;
    }
    return true;
}

MeshHandle Renderer::CreateCube() {
    Impl& r = *impl_;
    Mesh mesh;
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = sizeof(kCubeVertices);
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = kCubeVertices;
    if (!step::Check(r.device->CreateBuffer(&vbDesc, &vbData, &mesh.vertexBuffer), "CreateBuffer(vertex)")) {
        return {};
    }
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.ByteWidth = sizeof(kCubeIndices);
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = kCubeIndices;
    if (!step::Check(r.device->CreateBuffer(&ibDesc, &ibData, &mesh.indexBuffer), "CreateBuffer(index)")) {
        return {};
    }
    mesh.indexCount = ARRAYSIZE(kCubeIndices);
    r.meshes.push_back(mesh);
    return MeshHandle{static_cast<int>(r.meshes.size()) - 1};
}

void Renderer::BeginFrame(const Vector4& clearColor) {
    Impl& r = *impl_;
    if (!r.context) return;
    const float color[4] = {clearColor.x, clearColor.y, clearColor.z, clearColor.w};
    r.context->ClearRenderTargetView(r.renderTargetView.Get(), color);
    r.context->ClearDepthStencilView(r.depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    r.context->OMSetRenderTargets(1, r.renderTargetView.GetAddressOf(), r.depthStencilView.Get());

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(r.width);
    viewport.Height = static_cast<float>(r.height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    r.context->RSSetViewports(1, &viewport);

    r.context->IASetInputLayout(r.inputLayout.Get());
    r.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    r.context->VSSetShader(r.vertexShader.Get(), nullptr, 0);
    r.context->PSSetShader(r.pixelShader.Get(), nullptr, 0);
    r.context->VSSetConstantBuffers(0, 1, r.constantBuffer.GetAddressOf());
    r.context->PSSetConstantBuffers(0, 1, r.constantBuffer.GetAddressOf());
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
    if (!r.context || !handle.IsValid() || handle.index >= static_cast<int>(r.meshes.size())) return;
    const Mesh& mesh = r.meshes[handle.index];

    PerObjectConstants constants;
    constants.worldViewProj = (world * r.view * r.projection).Transpose();
    constants.world = world.Transpose();
    constants.color = color;
    constants.lightDirection = r.lightDirection;
    r.context->UpdateSubresource(r.constantBuffer.Get(), 0, nullptr, &constants, 0, 0);

    const UINT stride = sizeof(Vertex);
    const UINT offset = 0;
    r.context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
    r.context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    r.context->DrawIndexed(mesh.indexCount, 0, 0);
}

void Renderer::Present() {
    if (impl_->swapChain) impl_->swapChain->Present(1, 0);
}

bool Renderer::SaveBackBufferPng(const std::wstring& path) {
    return step::SaveTexturePng(impl_->context.Get(), impl_->backBuffer.Get(), path);
}

float Renderer::AspectRatio() const {
    return impl_->height == 0 ? 1.0f : static_cast<float>(impl_->width) / static_cast<float>(impl_->height);
}

}  // namespace render
