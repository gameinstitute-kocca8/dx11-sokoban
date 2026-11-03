# 11 AI 사용 범위

## 수정 대상

`Render/Renderer.cpp`, `Game/SokobanGame.cpp`, `Game/main.cpp`의 `TODO(11)`

## 아직 모르는 개념 (AI가 꺼내지 않아야 하는 것)

- UI 프레임워크(ImGui 등), 위젯, 이벤트 기반 UI
- 런타임 글꼴 래스터화, 한글 글꼴
- Undo (12)

## 좋은 질문 예

- "글자는 보이는데 두 번째 프레임부터 바닥 뒤의 벽이 앞에 그려진다. BeginFrame은 이렇다. SpriteBatch가 바꾸는 상태 중 무엇을 확인해야 할까?"
