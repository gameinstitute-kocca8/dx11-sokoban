# 01 통과 조건

## 동작

- [ ] Debug 빌드 실행 시 창 전체가 남색
- [ ] 출력 창에 `[실패]` 로그가 없음
- [ ] `--capture`로 찍은 화면이 `expected/01_ClearColor.png`와 같음 (CI 아티팩트 `my-screens`)

## 기록

- [ ] SPEC 커밋 → 구현 커밋 순서
- [ ] 예측 질문 3개의 사전 답과 실제 결과
- [ ] `LOG.md` 1~2건

## 데모에서 설명할 수 있어야 하는 것

- Device와 DeviceContext를 왜 나눠 두었을까? (만드는 일 / 시키는 일)
- 백 버퍼 텍스처와 Render Target View는 무엇이 다른가?
- 디버그 레이어가 없는 PC에서 `D3D11_CREATE_DEVICE_DEBUG`를 켜면 어떻게 되고, 코드는 그것을 어떻게 처리하는가?
