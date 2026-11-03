# DX11 미니 엔진 + 3D 소코반

DirectX 11로 작은 3D 소코반을 만들면서, 게임에 **실제로 필요한** 엔진 구성 요소만 직접 만들어 보는 수업입니다.

이 수업의 목표는 상용 엔진을 닮은 무언가를 만드는 것이 아닙니다. 최종적으로 자기 코드의 **목적, 경계, 고장 원인, 수정 방법**을 스스로 설명할 수 있으면 됩니다.

```text
게임 규칙 먼저 (00, 콘솔)  →  그리는 법 (01~06)  →  둘을 잇는 최소 엔진 (07~)  →  3D 소코반
```

## 진행 방식

- **한 단계에 한 개념, 한 확인 화면.** 각 단계 폴더에 목표(`TASK.md`), 통과 조건(`CHECKLIST.md`), 그 단계의 AI 사용 범위(`AI-TUTOR.md`), 기대 결과(`expected/`)가 있습니다.
- **starter는 '이전 단계까지 동작하는 코드 + 이번 단계의 `TODO`'입니다.** `TODO(NN)` 주석을 채우는 것이 과제입니다.
- **기본은 자기 코드로 계속 진행하는 것입니다.** 막혔을 때만 다음 단계의 starter로 갈아탑니다. 최종 시연은 자기 코드에서 합니다.
- **정답은 단계 마감 후 `answers/`에 공개됩니다.** 해설을 읽고 자기 구현과 비교하는 복기 자료입니다.

## 단계

| 단계 | 개념 | 확인 결과 | 상태 |
|---|---|---|---|
| [00_TextSokoban](steps/00_TextSokoban/TASK.md) | 게임 상태, 상태 전이, 시나리오 테스트 | 콘솔 소코반 + 테스트 전부 PASS | ✅ |
| [01_ClearColor](steps/01_ClearColor/TASK.md) | Device, SwapChain, Render Target, Present | 단색 창 | ✅ |
| [02_Triangle](steps/02_Triangle/TASK.md) | Vertex Buffer, Input Layout, HLSL, Draw | 색 삼각형 | ✅ |
| [03_IndexDepth](steps/03_IndexDepth/TASK.md) | Index Buffer, Depth Buffer | 겹친 사각형의 앞뒤가 올바름 | ✅ |
| [04_Transform](steps/04_Transform/TASK.md) | World/View/Projection, Constant Buffer | 회전하는 큐브와 바닥 | ✅ |
| [05_Camera](steps/05_Camera/TASK.md) | Camera 클래스 (View, Projection), 궤도 | 3×3 큐브 주위를 도는 카메라 | ✅ |
| 06_Lighting | 법선, 방향광 (Lambert) | 면마다 밝기가 다른 큐브 | 준비 중 |
| 07_SplitLibraries | `Render.lib` / `Engine.lib` 분리 | CubeViewer.exe가 따로 빌드됨 | 준비 중 |
| 08_GridMap | 00의 레벨 → 3D 맵 | 레벨 파일이 벽·바닥·목표로 보임 | 준비 중 |
| 09_PlayerInput | 키 입력(눌린 순간), 3인칭 카메라 | 플레이어가 한 칸씩 이동 | 준비 중 |
| 10_PushAndClear | 00의 규칙 연결, 승리 판정 | 이동 / 밀기 / 막힘 / Clear | 준비 중 |
| 11_UIRestart | SpriteFont, 이동 횟수, R 재시작 | 화면 위 텍스트 | 준비 중 |
| 12_Optional | Undo, 부드러운 이동, 텍스처, 다중 레벨 중 택1 | 선택 기능 동작 | 준비 중 |

## 빠른 시작

1. **이 레포를 fork하지 마세요.** [docs/WORKFLOW.md의 "0. 내 레포 만들기"](docs/WORKFLOW.md)대로 템플릿에서 내 Private 레포를 만들고 clone합니다.
2. [docs/SETUP.md](docs/SETUP.md)를 따라 Visual Studio와 CMake 환경을 준비합니다.
3. Visual Studio에서 **파일 → 열기 → 폴더**로 내 레포 폴더를 엽니다.
4. 시작 항목으로 `TextSokobanTests.exe`를 골라 실행합니다. 처음에는 테스트가 실패하는 것이 정상입니다. GitHub Actions의 규칙 테스트가 빨간색인 것도 같은 이유입니다.
5. [steps/00_TextSokoban/TASK.md](steps/00_TextSokoban/TASK.md)부터 시작합니다.

## 꼭 읽을 문서

| 문서 | 내용 |
|---|---|
| [docs/COURSE.md](docs/COURSE.md) | 수업 원칙, 만들지 않는 것, 레이어 규칙 |
| [docs/WORKFLOW.md](docs/WORKFLOW.md) | 한 단계를 진행하는 순서 (SPEC → 구현 → 검증 → 제출) |
| [docs/AI-TUTOR.md](docs/AI-TUTOR.md) | AI에게 묻는 방법과 규칙 |
| [docs/ASSESSMENT.md](docs/ASSESSMENT.md) | 평가 방법 |
| [docs/LEVEL-FORMAT.md](docs/LEVEL-FORMAT.md) | 소코반 레벨 파일 형식 |

## 레포 구조

```text
steps/NN_이름/
  TASK.md          목표, 입력, 출력, 금지 범위, 예측 질문
  CHECKLIST.md     통과 조건
  AI-TUTOR.md      이 단계의 AI 사용 범위
  starter/         여기서 작업한다
  expected/        기대 결과 (PNG 또는 텍스트)
common/            창, 메시지 루프, 로그, 캡처 같은 도구 코드 (수업 개념 아님)
templates/         SPEC, 질문, 학습 기록 양식
tools/             레이어 검사, 화면 캡처 스크립트
answers/           단계 마감 후 공개되는 정답과 해설
```
