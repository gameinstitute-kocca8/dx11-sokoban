# 09 AI 사용 범위

## 수정 대상

`Engine/Input.cpp`, `Engine/Application.cpp`, `Game/PlayerController.cpp`, `Game/main.cpp`의 `TODO(09)`

## 아직 모르는 개념 (AI가 꺼내지 않아야 하는 것)

- 입력 액션 매핑, 이벤트 버스, 콜백/옵저버 기반 입력 시스템
- Raw Input, 게임패드, GameInput
- 이동 애니메이션, 보간 (12)

## 좋은 질문 예

- "키를 한 번 눌렀는데 3~4칸 움직인다. ReadMoveInput에서는 WasPressed를 쓴다. Input::Update는 이렇다. 원인 후보 두 개만."
- "카메라를 90도 돌리면 W가 오른쪽으로 간다. CameraRelativeDir의 앞 벡터 식은 이렇다. yaw = π/2일 때 앞 벡터가 무엇이어야 하는지부터 확인하고 싶다."
