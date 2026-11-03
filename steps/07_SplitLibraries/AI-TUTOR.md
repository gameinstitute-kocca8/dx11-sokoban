# 07 AI 사용 범위

## 수정 대상

`Render/Renderer.cpp`, `Engine/Application.cpp`, `CubeViewer/main.cpp`의 `TODO(07)`. 내용은 **06단계 내 코드에서 옮겨 온다.**

## 아직 모르는 개념 (AI가 꺼내지 않아야 하는 것)

- 렌더러 추상 인터페이스, 백엔드 교체, DLL/플러그인
- ECS, GameObject/Component, 씬 그래프, 리소스 매니저, 리플렉션
- 멀티스레드 렌더링, 커맨드 리스트

## 이 단계에서 특히 조심할 것

AI는 "엔진 구조"를 물으면 큰 프레임워크를 제안하기 쉽습니다. 이 단계의 구조는 이미 정해져 있습니다(`Render / Engine / CubeViewer`). **구조를 새로 설계해 달라고 묻지 마세요.** 옮기다가 생긴 빌드 오류나 화면 차이만 묻습니다.

## 좋은 질문 예

- "CubeViewer 빌드에서 LNK2019: render::Renderer::DrawMesh 미해결 외부 기호가 난다. CMake 링크 설정은 이렇다. 원인 후보 두 개만."
- "06과 비교하니 07 화면이 더 어둡다. SetLight를 부르는 순서는 이렇다. 무엇을 확인해야 할까?"
