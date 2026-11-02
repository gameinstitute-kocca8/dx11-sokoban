# 00 통과 조건

## 동작

- [ ] `TextSokobanTests.exe`: 제공된 시나리오가 모두 `[PASS]`
- [ ] 직접 추가한 시나리오 2개 이상이 `[PASS]`
- [ ] `TextSokoban.exe`로 `level01`~`level03`을 직접 클리어할 수 있음
- [ ] `TextSokoban.exe levels/level01.txt dd`의 출력이 `expected/level01_dd.txt`와 같음
- [ ] CI의 "규칙 테스트" 단계가 초록색

## 기록

- [ ] `SPEC.md`가 구현 커밋보다 먼저 커밋됨
- [ ] 예측 질문 3개에 대한 사전 답과 실제 결과
- [ ] `LOG.md` 1~2건

## 데모에서 설명할 수 있어야 하는 것

- Blocked일 때 상태가 바뀌지 않는다는 것을 코드의 어느 부분이 보장하는가?
- 목표 위의 상자를 다시 밀어냈을 때 그 칸이 다시 `.`이 되는 이유는? (Terrain과 Occupant)
- 직접 추가한 시나리오는 어떤 버그를 잡기 위해 만들었는가?
