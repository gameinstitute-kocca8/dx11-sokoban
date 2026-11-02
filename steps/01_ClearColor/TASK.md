# 01 Clear Color

## 목표

Direct3D 11의 최소 구성으로 창 전체를 한 가지 색으로 칠합니다.

## 배울 개념

| 객체 | 역할 |
|---|---|
| `ID3D11Device` | 리소스(버퍼, 텍스처, 셰이더)를 **만든다** |
| `ID3D11DeviceContext` | 그리기 명령을 **보낸다** |
| `IDXGISwapChain` | 백 버퍼와 화면을 **바꾼다** (Present) |
| `ID3D11RenderTargetView` | 텍스처를 "그리기 대상"으로 **본다** |

한 프레임: `ClearRenderTargetView` → (나중 단계: 그리기) → `Present`

## 할 일

`starter/main.cpp`의 `TODO(01)` 네 곳

1. `D3D11CreateDeviceAndSwapChain`
2. 백 버퍼 꺼내기 + Render Target View 만들기
3. `ClearRenderTargetView` (색: R 0.10, G 0.20, B 0.35)
4. `Present(1, 0)`

창 만들기와 메시지 루프는 `common/StepApp`이 합니다. 한 번은 읽어 보세요. 숨긴 것은 없습니다.

## 확인 화면

창 전체가 짙은 남색입니다 (`expected/01_ClearColor.png`).

## 예측 질문

1. `Present`를 호출하지 않으면 창에는 무엇이 보일까?
2. Clear 색의 알파를 0으로 바꾸면 창이 투명해질까?
3. `UseWarp()`가 true일 때와 false일 때 결과 화면이 다를까? 속도는?

## 금지 범위

- `IDXGIFactory`로 어댑터를 고르는 코드, 전체 화면 전환, 창 크기 변경 대응 (이 수업에서는 고정 크기)
