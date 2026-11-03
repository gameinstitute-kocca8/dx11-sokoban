# 05 Camera

## 목표

04단계에서 `OnUpdate` 안에 직접 만들던 View와 Projection 행렬을 **`Camera` 클래스** 하나로 묶습니다. 그리고 원점을 중심으로 천천히 도는 **궤도 카메라**를 만듭니다.

이 수업에서 처음 만드는 "엔진" 클래스입니다. 07단계에서 `Engine.lib`으로 옮겨지고, 소코반의 카메라(08~12)가 그대로 사용합니다.

## 배울 개념

```text
Camera
  SetLens(fovY, aspect, near, far)  → Projection 행렬
  LookAt(eye, target, up)           → View 행렬
  Orbit(target, distance, yaw, pitch)
      구면 좌표 → 눈 위치 → LookAt
```

- 카메라는 **행렬 두 개를 만드는 객체**일 뿐입니다. 키보드도, 셰이더도, 버퍼도 모릅니다.
- 구면 좌표: 거리 `d`, 수평 각 `yaw`, 높이 각 `pitch`로 눈 위치를 정합니다.
  - 수평 반지름 = `d·cos(pitch)`
  - 높이 = `d·sin(pitch)`
- `pitch`가 ±90도면 시선과 위쪽 벡터(+Y)가 평행해져 View 행렬을 만들 수 없습니다. 그래서 제한합니다.

## 할 일

| 파일 | 내용 |
|---|---|
| `starter/Camera.cpp` | `TODO(05)` 세 곳: `SetLens`, `LookAt`, `Orbit` |
| `starter/main.cpp` | `TODO(05)` 한 곳: `OnUpdate`에서 렌즈와 궤도 설정 |

`Camera.h`는 제공됩니다. 공개 함수를 늘리지 마세요. 필요하다고 느끼면 [NEED 규칙](../../docs/COURSE.md)에 따라 이유를 먼저 적습니다.

## 확인 화면

바닥 위에 3 × 3으로 놓인 큐브 9개를 위에서 비스듬히(35도) 내려다봅니다. 카메라가 원점을 중심으로 천천히 돕니다. CI 캡처는 `--time 1.0` 시점입니다 (`expected/05_Camera.png`).

## 예측 질문 (먼저 starter를 그대로 실행해 본다)

1. Camera를 채우기 전에는 View와 Projection이 단위 행렬입니다. 그러면 화면에 무엇이 보일까? 04단계 예측 질문 1번과 비교해 보자.
2. yaw = 0이면 카메라는 원점의 -Z 쪽에서 +Z 방향을 봅니다. 이때 **월드 +X 방향은 화면의 오른쪽일까, 왼쪽일까?** (08단계에서 레벨 파일의 x를 월드에 어떻게 놓을지 결정할 때 다시 씁니다)
3. `pitch`를 90도로 제한 없이 넣으면 화면은 어떻게 될까?
4. `SetLens`의 `nearZ`를 0으로 넣으면 디버그 빌드에서 어떻게 될까?
5. 화면비를 `Height / Width`로 거꾸로 넣으면 큐브는 어떻게 보일까?

## 금지 범위

- 키보드나 마우스로 카메라 움직이기 (09단계). 지금은 시간으로만 돕니다
- 카메라 클래스 계층(`PerspectiveCamera`, `OrthoCamera` ...), 카메라 매니저, 컴포넌트화
- 쿼터니언, 롤(roll) 회전, 자유 비행 카메라
