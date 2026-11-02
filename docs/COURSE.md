# 수업 원칙

## 왜 이 순서인가

엔진은 게임을 만들다가 같은 문제를 반복해서 만날 때 생기는 것입니다. 엔진을 먼저 만들고 나중에 게임을 얹으면 이렇게 됩니다.

- 쓰이지 않는 추상화가 먼저 생깁니다.
- 만든 사람도 그 추상화가 왜 있는지 설명하지 못합니다.
- 고장 났을 때 어디를 봐야 할지 모릅니다.

그래서 이 수업은 순서를 거꾸로 합니다.

1. **00**: 소코반 규칙을 콘솔에서 먼저 완성합니다. 게임은 첫 주부터 존재합니다.
2. **01~06**: Direct3D 11로 무언가를 그리는 법을 한 개념씩 배웁니다.
3. **07~**: 이미 동작하는 게임 규칙에 3D 화면을 붙입니다. 엔진 코드는 이때 **필요해진 만큼만** 만듭니다.

## 만들지 않는 것

아래 항목은 기본 과제에서 만들지 않습니다. 멋져 보이지만, 이 게임의 어떤 요구에서도 나오지 않습니다.

- 리플렉션, 코드 생성, Clang AST 도구, `UCLASS`/`UPROPERTY` 같은 매크로 표면
- 자체 에디터, 인스펙터
- 렌더러 백엔드 교체 추상화 (`IRenderer`, 플러그인)
- ECS, 범용 이벤트 버스, 범용 GameObject/Component 체계
- 자체 수학 타입 (`FVector` 같은 것). 수학은 DirectXTK `SimpleMath`를 씁니다
- 물리 엔진, 네트워크, 멀티스레드
- 게임 요구와 연결되지 않은 JSON 저장/직렬화

## NEED 규칙

엔진(`Render`, `Engine`)에 새 클래스나 공개 함수를 추가하는 커밋에는 다음 한 줄을 반드시 적습니다.

```text
NEED: 09단계에서 [구체적인 실패나 불편] 때문에 추가
```

이 줄을 쓸 수 없다면, 그 코드는 아직 필요하지 않은 것입니다.

## 레이어 (07단계부터)

```text
Game  →  Engine  →  Render
```

| 레이어 | 들어가는 것 | 들어가면 안 되는 것 |
|---|---|---|
| `Render.lib` | Device, SwapChain, Shader, Mesh, Texture, Material, Font, DebugDraw | Engine/Game 헤더, 게임 단어(Sokoban, Crate, Goal, Player) |
| `Engine.lib` | Application(루프), Time, Input, Camera | Game 헤더, 게임 단어 |
| `Sokoban.exe` | 레벨, 규칙, 플레이어 조작, UI | `d3d11.h`, `dxgi.h`, 텍스처 로더 |

**Engine에 무엇을 넣을지는 다음 기준으로 판단합니다.**

> `CubeViewer.exe`와 `Sokoban.exe` **둘 다** 쓰는 것만 Engine에 넣는다.

- Camera, Input, Time은 둘 다 쓰므로 Engine에 넣습니다.
- 범용 GameObject는 둘 다 쓰지 않으므로 넣지 않습니다. 소코반은 그리드 상태를 매 프레임 그대로 그리면 됩니다.

의존 방향은 `tools/check_layers.py`가 CI에서 자동으로 검사합니다.

## 가장 중요한 설계 원칙: 상태가 진실이다

```text
Level (Terrain[] + Occupant[] + player)    ← 게임의 모든 상태
      │  매 프레임
      ▼
for each cell: DrawCube(cellToWorld(x, y), colorOf(cell))
```

상자를 객체로 만들어 위치를 따로 들고 있으면, 그리드 상태와 화면 위치가 어긋나는 버그가 생깁니다. 화면은 상태에서 **계산**합니다. 이렇게 하면 Restart는 초기 상태 복사, Undo는 상태 스택이 됩니다.

## 도구 코드 (`common/`)

창 만들기, 메시지 루프, 로그, 경로, PNG 캡처처럼 매 단계 반복되지만 수업 개념이 아닌 코드는 `common/StepApp`에 있습니다. Direct3D 객체는 `common/`에 절대 없습니다. 각 단계의 `main.cpp`에서 직접 만듭니다.

## 디버깅 도구

AI에게 묻기 전에 다음 도구를 먼저 씁니다.

- **D3D11 디버그 레이어**: Debug 빌드에서 켜져 있습니다. 경고와 오류가 Visual Studio 출력 창에 나옵니다. 설치는 [SETUP.md](SETUP.md)를 참고하세요.
- **`step::Log` / `step::Check`**: 어떤 호출이 실패했는지 출력 창에 남깁니다.
- **RenderDoc**: 화면이 비어 있을 때 파이프라인 각 단계의 실제 상태를 확인합니다.
