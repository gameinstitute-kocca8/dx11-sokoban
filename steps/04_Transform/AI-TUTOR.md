# 04 AI 사용 범위

## 수정 대상

`main.cpp`의 `TODO(04)`, `shaders/Transform.hlsl`의 `VSMain`

## 아직 모르는 개념

- Camera 클래스, 입력 (05, 09)
- 조명, 법선, 텍스처 (06, 12)
- 인스턴싱, 동적 버퍼(`D3D11_USAGE_DYNAMIC` + Map), 여러 Constant Buffer 슬롯 설계
- 자체 수학 라이브러리, Transform 컴포넌트, 씬 그래프

## 좋은 질문 예

- "큐브가 화면에 안 보인다. `world * view * projection`을 Transpose해서 넣고 있다. HLSL은 `mul(WorldViewProj, pos)`로 썼다. 이 순서가 맞는지 설명해 달라."
