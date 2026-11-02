# 04 Transform & Constant Buffer

## 목표

정육면체 메시 하나를 **World 행렬만 바꿔** 두 번 그립니다. 하나는 회전하는 큐브, 하나는 납작하게 늘린 바닥입니다.

이것이 08단계 그리드 맵의 출발점입니다. 맵의 벽 100개는 "같은 큐브를 World 행렬만 바꿔 100번 그리기"입니다.

## 배울 개념

```text
모델 좌표 ─(World)→ 월드 좌표 ─(View)→ 카메라 좌표 ─(Projection)→ 클립 좌표
```

- `SimpleMath::Matrix`: **오른손 좌표계**, **행 벡터 × 행렬** 규약. 곱하는 순서는 `world * view * projection`
- Constant Buffer: 그리기마다 바뀌는 값(행렬, 색)을 셰이더로 보내는 작은 버퍼. 크기는 16바이트의 배수
- HLSL 행렬은 기본이 column-major입니다. 그래서 C++에서 `Transpose()`해서 넣습니다

## 할 일

| 파일 | 내용 |
|---|---|
| `starter/main.cpp` | `TODO(04)` 네 곳: 상수 구조체, Constant Buffer 생성, 행렬 계산(`OnUpdate`), 그리기마다 상수 갱신(`DrawCube`) |
| `starter/shaders/Transform.hlsl` | `TODO(04)`: `mul`로 위치 변환, Tint 곱하기 |

## 확인 화면

면마다 색이 다른 큐브가 회전하고, 아래에 어두운 바닥이 있습니다. CI 캡처는 `--time 1.0` 시점입니다 (`expected/04_Transform.png`).

## 예측 질문 (먼저 starter를 그대로 실행해 본다)

1. 변환 없이 큐브(모델 좌표 ±0.5)를 그리면 무엇이 보일까? 여섯 면 각각이 어떻게 되는지(잘림, 컬링, 옆에서 본 면) 설명해 보자.
2. `Transpose()`를 빼면?
3. `world * view * projection`을 `projection * view * world`로 바꾸면?
4. 바닥의 `CreateScale`과 `CreateTranslation` 순서를 바꾸면 바닥은 어디로 갈까?
5. Constant Buffer 하나를 두 물체가 같이 쓰는데, 왜 둘 다 올바른 위치에 그려질까?

## 금지 범위

- Camera 클래스, 입력으로 카메라 움직이기 (05, 09)
- 조명, 법선 (06)
- 자체 수학 타입 (`FVector`, `FMatrix`). `SimpleMath`를 그대로 씁니다
- 물체마다 Constant Buffer 따로 만들기. 하나를 갱신하며 재사용합니다
