# 02 Triangle

## 목표

정점 3개를 GPU로 보내 색 삼각형 하나를 그립니다.

## 배울 개념

```text
C++ Vertex 구조체  ──  Input Layout  ──  HLSL VSInput      (같은 메모리 배치를 세 곳에서 적는다)
        │
Vertex Buffer → IA(조립) → VS(정점 셰이더) → RS(래스터라이저, 뷰포트) → PS(픽셀 셰이더) → OM(렌더 타깃)
```

- 클립 공간 좌표: x, y는 -1~1 (오른쪽 위가 +), z는 0~1
- 앞면 판정: 화면에서 **시계 방향**으로 놓인 삼각형만 그린다 (D3D11 기본 래스터라이저 상태)

## 할 일

| 파일 | 내용 |
|---|---|
| `starter/main.cpp` | `TODO(02)` 세 곳: 정점 버퍼, 셰이더와 Input Layout, 파이프라인 설정과 `Draw` |
| `starter/shaders/Triangle.hlsl` | `TODO(02)`: `VSMain`에서 위치와 색 전달 |

`step::CompileShader`는 컴파일 오류를 파일, 줄 번호와 함께 출력 창에 남깁니다.

## 확인 화면

남색 배경 가운데에 위쪽 빨강, 오른쪽 아래 초록, 왼쪽 아래 파랑인 삼각형이 있고, 내부는 색이 보간되어 있습니다 (`expected/02_Triangle.png`).

## 예측 질문

1. 정점 순서를 위 → **왼쪽 아래** → 오른쪽 아래로 바꾸면?
2. Input Layout의 COLOR 오프셋을 12가 아니라 16으로 적으면?
3. 뷰포트를 설정하지 않으면?
4. PS에서 `return float4(1, 1, 1, 1);`로 바꾸면? 보간은 어디서 일어나는가?

## 금지 범위

- 인덱스 버퍼, 행렬, Constant Buffer (03~04)
- 셰이더 파일 자동 재컴파일, 셰이더 리플렉션
