# 한 단계를 진행하는 순서

```text
읽기 → 예측 → SPEC 커밋 → 구현 → 검증 → 기록 → 제출 → 데모 → 정답 비교
```

## 1. 읽기

`steps/NN_이름/TASK.md`와 `CHECKLIST.md`를 읽고, `expected/`의 기대 결과를 확인합니다.

## 2. 예측

`TASK.md`의 **예측 질문**에 실행하기 전에 답을 적습니다. 틀려도 괜찮습니다. 나중에 실행 결과와 비교하는 것이 목적입니다.

## 3. SPEC 커밋 (코드보다 먼저)

`templates/SPEC.md`를 `steps/NN_이름/SPEC.md`로 복사해 채우고, **코드를 고치기 전에** 커밋합니다.

```bash
git add steps/03_IndexDepth/SPEC.md
git commit -m "03 SPEC"
```

이 커밋 순서가 평가 자료입니다. SPEC이 구현보다 먼저 있어야 합니다.

## 4. 구현

`starter/` 안의 `TODO(NN)`을 채웁니다. TODO 바깥은 원칙적으로 고치지 않습니다. 고쳐야 한다면 SPEC에 이유를 적습니다.

## 5. 검증

- 직접 실행해서 `expected/`와 비교합니다.
- 예측 질문을 실제로 실험해 봅니다. 예: 깊이 버퍼를 끄면 어떻게 되는가?
- push하면 CI가 실제 화면을 찍어 Actions 아티팩트 `my-screens`로 올립니다.

## 6. 기록

`templates/LOG.md` 형식으로 `steps/NN_이름/LOG.md`에 **1~2건**만 적습니다. 막혔던 것, AI에게 물은 것, 내가 내린 판단, 검증 결과를 남깁니다. 전체 채팅 로그는 필요 없습니다.

## 7. 제출

```bash
git tag step03-submit
git push --tags
```

## 8. 데모

강사 앞에서 실행하고 `CHECKLIST.md`의 질문에 답합니다.

## 9. 정답 비교

단계가 마감되면 `answers/NN_이름/`에 정답과 해설(`NOTES.md`)이 올라옵니다. 내 코드와 비교해 봅니다.

```bash
git pull
git diff --no-index steps/03_IndexDepth/starter answers/03_IndexDepth
```

## 막혔을 때

1. 출력 창의 `[실패]` 로그와 D3D 디버그 레이어 메시지를 봅니다.
2. RenderDoc으로 프레임을 캡처합니다.
3. `templates/QUESTION.md` 양식으로 질문을 정리합니다. 정리하다가 답이 보이는 경우가 많습니다.
4. 그래도 안 되면 AI나 강사에게 물어봅니다 ([AI-TUTOR.md](AI-TUTOR.md)).
5. 다음 단계로 넘어가야 하는데 이 단계가 끝나지 않았다면, 다음 단계의 `starter/`에서 시작합니다. 그 starter에는 이 단계까지의 동작하는 코드가 들어 있습니다.
