# 06 AI 사용 범위

## 수정 대상

`main.cpp`의 `TODO(06)`, `shaders/Lit.hlsl`의 `VSMain`과 `PSMain`

## 아직 모르는 개념 (AI가 꺼내지 않아야 하는 것)

- 반사광(Phong, Blinn-Phong), PBR, 그림자 맵, 노멀 맵
- 텍스처, 샘플러 (12단계 선택 과제)
- 여러 Constant Buffer 슬롯 설계, 조명 시스템, 재질 클래스

## 좋은 질문 예

- "모든 면이 같은 밝기로 보인다. PS에서 diffuse를 색으로 출력해 보니 0.8로 일정했다. VS에서 법선이 넘어오는 부분은 이렇다. 원인 후보 두 개만."
