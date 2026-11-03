# 07 Split Libraries

## 목표

06단계 한 파일에 있던 코드를 **세 덩어리로 나눕니다.** 새 기능은 없습니다. 같은 화면을, 엔진을 재사용하는 두 번째 프로그램(`CubeViewer`)으로 다시 그립니다.

```text
07_SplitLibraries/
  Render/      Render.lib  : Direct3D 11을 아는 유일한 곳 (Renderer, 셰이더)
  Engine/      Engine.lib  : 창과 루프(Application), Camera. Render만 안다
  CubeViewer/  CubeViewer.exe : Engine만 안다. d3d11.h가 없다
```

## 왜 지금 나누는가

01~06단계의 `main.cpp`는 매번 Device를 만들고, 지우고, Present했습니다. 08단계부터는 같은 일을 하는 소코반 프로그램이 생깁니다. 지금 나누지 않으면 같은 코드를 두 번 관리해야 합니다. 이것이 이 단계의 **NEED**입니다.

## 배울 개념

- **정적 라이브러리**와 의존 방향: `CubeViewer → Engine → Render`. 반대 방향 `#include`는 CI의 레이어 검사(`tools/check_layers.py`)가 막습니다.
- **헤더로 경계 숨기기:** `Renderer.h`에는 `d3d11.h`가 없습니다. Direct3D 객체는 `Renderer.cpp` 안의 `Impl` 구조체에 숨깁니다(pimpl).
- **Engine에 넣을 것의 기준:** CubeViewer와 Sokoban이 **둘 다** 쓰는 것만 넣습니다 ([docs/COURSE.md](../../docs/COURSE.md)).

## 할 일: 06단계 코드를 '옮겨' 채우기

| 파일 | `TODO(07)` | 06단계의 어디서 오는가 |
|---|---|---|
| `Render/Renderer.cpp` | `Initialize` | `OnInit`의 Device ~ Constant Buffer |
| | `CreateCube` | `OnInit`의 정점/인덱스 버퍼 |
| | `BeginFrame` | `OnRender` 앞부분 |
| | `DrawMesh` | `DrawCube` (+ 메시 버퍼 바인딩) |
| | `Present` | `OnPresent` |
| `Engine/Application.cpp` | `OnInit`, `OnRender` | Renderer 초기화와 프레임 순서 |
| `CubeViewer/main.cpp` | 세 곳 | 06단계 `OnUpdate`, `OnRender`의 장면 |

`Engine/Camera.*`는 05단계 Camera를 그대로 옮긴 것입니다 (`namespace engine`만 추가). CMake 설정도 제공됩니다.

처음 받은 starter는 `Application::OnInit`이 비어 있어서 **초기화 실패가 정상**입니다.

## 확인 화면

06단계와 **같은 화면**입니다 (`expected/07_CubeViewer.png`). 픽셀까지 같아야 합니다. 다르면 옮기다가 무언가 빠진 것입니다.

## 예측 질문

1. `Renderer.h`에 `#include <d3d11.h>`를 넣으면 무엇이 나빠질까? 빌드는 될까?
2. `Render/Renderer.cpp`에서 `#include "Engine/Camera.h"`를 하면 CI의 어느 단계가 실패할까?
3. `CubeViewer/CMakeLists.txt`에서 `07_Engine` 대신 `07_Render`만 링크하면 어떤 오류가 날까?
4. `Application`의 `OnInit`, `OnRender` 등을 `final`로 막은 이유는?
5. 셰이더 파일(`Render/shaders`)은 Render의 것인데 왜 CubeViewer 실행 파일 옆으로 복사될까?

## 금지 범위

- 렌더러 교체 인터페이스(`IRenderer`), 플러그인, DLL
- GameObject, Component, Scene 그래프 (docs/COURSE.md의 판단 기준으로 탈락)
- 06단계에 없던 기능 추가. 이 단계는 **옮기기만** 한다
