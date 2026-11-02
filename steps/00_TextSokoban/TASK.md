# 00 콘솔 소코반

## 목표

DirectX 없이 소코반 **규칙**을 완성합니다. 이 코드(`Sokoban.h/.cpp`)는 10단계에서 3D 게임에 **그대로** 연결됩니다.

## 배울 개념

- 게임 상태를 두 층(Terrain / Occupant)으로 표현하기
- 상태 전이: `TryMove(level, dir)` → `Moved` / `Pushed` / `Blocked`
- "판정 먼저, 변경은 나중": Blocked이면 상태가 전혀 바뀌지 않아야 한다
- 시나리오 테스트로 규칙 검증하기

## 할 일

| 파일 | 내용 |
|---|---|
| `starter/Sokoban.cpp` | `TODO(00)` 두 곳: `TryMove`, `IsCleared` |
| `starter/tests.cpp` | 맨 아래 `MY SCENARIOS`에 직접 만든 시나리오 **2개 이상** 추가 |

레벨 읽기/쓰기(`LoadLevelFromText`, `ToText`)는 제공됩니다. 형식은 [docs/LEVEL-FORMAT.md](../../docs/LEVEL-FORMAT.md)를 참고하세요.

## 입력과 출력

- 입력: 레벨 텍스트, 방향 키 `w a s d`
- 출력: 각 입력의 결과(`M`/`P`/`B`), 최종 맵 텍스트, Clear 여부

## 실행

- `TextSokobanTests.exe`: 시나리오 테스트. 기대 결과는 `expected/tests.txt`
- `TextSokoban.exe`: 직접 플레이 (`levels/level01.txt` ~ `level03.txt`)
- `TextSokoban.exe levels/level01.txt dd`: 입력을 실행하고 최종 상태만 출력. 기대 결과는 `expected/level01_dd.txt`

## 예측 질문 (실행 전에 SPEC.md에 답을 적는다)

1. `TryMove`에서 상자를 옮기기 **전에** 플레이어를 먼저 옮기면, 어떤 시나리오가 실패할까?
2. `IsCleared`가 "상자가 모두 목표 위에 있는가"를 검사하도록 만들면, 상자 수와 목표 수가 다른 레벨에서 무슨 일이 생길까?
3. 맵 밖 좌표를 벽으로 취급하지 않으면 어떤 입력에서 프로그램이 죽을까?

## 금지 범위

- 렌더링, 파일 저장, Undo, 다중 레벨 관리 (나중 단계 또는 선택 과제)
- 규칙을 클래스 계층(예: `Entity` → `Crate`)으로 만들기. 두 층의 배열이면 충분합니다
