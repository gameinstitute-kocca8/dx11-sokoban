# 11 UI & Restart

## 목표

화면에 글자를 그리고(HUD), R로 재시작, N/P로 레벨을 바꿉니다.

## 배울 개념

- **3D 위에 2D 그리기:** 글자는 3D를 다 그린 **뒤**에 그려야 맨 위에 보입니다. 그래서 `DrawScreenText`는 바로 그리지 않고 모아 두었다가 `EndFrame`에서 한 번에 그립니다.
- **파이프라인 상태는 남는다:** DirectXTK `SpriteBatch`는 글자를 그리려고 깊이 테스트를 끄고 알파 블렌드를 켭니다. 다음 프레임 시작(`BeginFrame`)에서 기본값으로 되돌리지 않으면 3D가 깨집니다.
- **HUD는 상태를 읽기만 한다:** 화면 글자는 `Moves`, `State`, 레벨 번호를 읽어 매 프레임 다시 씁니다.
- **재시작 = 처음 상태 복사:** 상태가 값(`Level`)이라 `initial_`을 복사하면 끝입니다.

글꼴 파일 `Render/fonts/ui.spritefont`는 제공됩니다 (DejaVu Sans Mono, ASCII만). 화면 글자는 영어로만 씁니다.

## 할 일

| 파일 | `TODO(11)` |
|---|---|
| `Render/Renderer.cpp` | `BeginFrame` 상태 되돌리기, `LoadFont`, `EndFrame` |
| `Game/SokobanGame.cpp` | `Restart` |
| `Game/main.cpp` | 글꼴 읽기, R/N/P 처리, HUD |

## 확인 화면 (CI: `ci/captures.txt`, level01)

| 이름 | 실행 옵션 | 기대 |
|---|---|---|
| `11_Sokoban_start` | (없음) | 왼쪽 위 `Level 1/3   Moves: 0`, 아래쪽 조작 안내 |
| `11_Sokoban_clear` | `--moves dd` | 가운데 큰 `CLEAR!`와 `Moves: 2   N: Next level` |

## 예측 질문

1. `BeginFrame`에서 상태를 되돌리지 않으면 **첫 프레임**은 정상일까? 두 번째 프레임부터는?
2. HUD를 3D보다 먼저 그리면 어떻게 보일까?
3. `Restart`에서 `initial_` 대신 레벨 파일을 다시 읽으면 무엇이 다를까? (결과, 속도, 실패 가능성)
4. 글자에 한글을 넣으면 어떻게 보일까? 이유는? (글꼴 파일에 무엇이 들어 있는가)

## 금지 범위

- UI 프레임워크, 위젯, 레이아웃 엔진, 버튼 클릭
- 런타임 TTF 렌더링, 한글 글꼴 (글꼴 파일이 커지고 이 단계의 개념과 무관)
- 레벨 선택 메뉴 화면 (N/P 키로 충분)
