# 진행 방법

## 0. 내 레포 만들기 (처음 한 번)

수업 레포는 수업용 GitHub Organization 안에 있는 **템플릿**입니다. fork하지 않습니다.

1. 수업 레포 페이지에서 **Use this template → Create a new repository**를 누릅니다.
2. 다음과 같이 설정합니다.
   - **Owner**: 수업 Organization (내 개인 계정이 아님)
   - **Repository name**: `dx11-sokoban-내GitHub아이디`
   - **Private**
3. 만든 레포를 PC에 clone하고, **작업을 시작하기 전에** 원본 레포를 `upstream`으로 연결합니다. 원본 주소는 내 레포 페이지의 이름 아래 **"generated from ..."**에 적혀 있습니다.

```powershell
git clone https://github.com/<수업 Organization>/dx11-sokoban-내GitHub아이디.git
cd dx11-sokoban-내GitHub아이디
git remote add upstream https://github.com/<수업 Organization>/dx11-sokoban.git
git pull --no-rebase upstream main --allow-unrelated-histories --no-edit
git push
```

마지막 `pull`은 **아무것도 고치기 전에 한 번** 해야 합니다. 템플릿으로 만든 레포는 원본과 커밋 기록이 이어져 있지 않습니다. 이 첫 `pull`이 둘을 이어 줘서, 이후 업데이트가 충돌 없이 들어옵니다. 작업을 시작한 뒤에 처음 `pull`하면 내가 고친 파일마다 충돌이 납니다.

Organization 안에서 만든 레포는 강사가 볼 수 있고, 다른 학생은 볼 수 없습니다.

## 원본 업데이트 받기 (새 단계, 정답 공개)

새 단계나 `answers/`가 원본에 올라오면 강사가 알려 줍니다. 그때 다음을 실행합니다.

```powershell
git pull --no-rebase upstream main --no-edit
git push
```

원본 업데이트는 새 파일과 문서만 바꾸고, 이미 있는 `starter/`는 건드리지 않습니다. 그래서 내 작업과 충돌하는 일은 거의 없습니다. 충돌이 나면 혼자 해결하지 말고 강사에게 알립니다.

---

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

- 직접 실행해서 `expected/`와 비교합니다. 같은 방법으로 찍으려면 `--capture` 옵션을 씁니다 ([SETUP.md](SETUP.md)).
- 예측 질문을 실제로 실험해 봅니다. 예: 깊이 버퍼를 끄면 어떻게 되는가?
- push할 때마다 CI가 00 규칙 테스트와 레이어 검사를 돌립니다 (Linux, 1분 안팎).

## 6. 기록

`templates/LOG.md` 형식으로 `steps/NN_이름/LOG.md`에 **1~2건**만 적습니다. 막혔던 것, AI에게 물은 것, 내가 내린 판단, 검증 결과를 남깁니다. 전체 채팅 로그는 필요 없습니다.

## 7. 제출

```bash
git tag step03-submit
git push --tags
```

제출 태그를 push하면 CI가 Windows에서 전체를 빌드하고, 각 단계 화면을 찍어 Actions 아티팩트 `my-screens`로 올립니다. 이 화면이 제출 증거입니다.

Windows CI는 조직 전체가 함께 쓰는 사용량에서 차감됩니다. 그래서 제출할 때만 자동으로 돌고, 평소 확인은 내 PC에서 합니다. 고친 뒤 다시 제출하려면 태그 이름을 바꿉니다 (예: `step03-submit2`).

## 8. 데모

강사 앞에서 실행하고 `CHECKLIST.md`의 질문에 답합니다.

## 9. 정답 비교

단계가 마감되면 `answers/NN_이름/`에 정답과 해설(`NOTES.md`)이 올라옵니다. 내 코드와 비교해 봅니다.

```bash
git pull --no-rebase upstream main --no-edit
git diff --no-index steps/03_IndexDepth/starter answers/03_IndexDepth
```

## 막혔을 때

1. 출력 창의 `[실패]` 로그와 D3D 디버그 레이어 메시지를 봅니다.
2. RenderDoc으로 프레임을 캡처합니다.
3. `templates/QUESTION.md` 양식으로 질문을 정리합니다. 정리하다가 답이 보이는 경우가 많습니다.
4. 그래도 안 되면 AI나 강사에게 물어봅니다 ([AI-TUTOR.md](AI-TUTOR.md)).
5. 다음 단계로 넘어가야 하는데 이 단계가 끝나지 않았다면, 다음 단계의 `starter/`에서 시작합니다. 그 starter에는 이 단계까지의 동작하는 코드가 들어 있습니다.
