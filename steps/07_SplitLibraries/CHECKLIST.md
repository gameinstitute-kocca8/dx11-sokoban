# 07 통과 조건

## 동작

- [ ] `07_CubeViewer.exe`가 06단계와 같은 화면을 그림 (`--time 1.0` 캡처 비교)
- [ ] `CubeViewer/` 폴더 어디에도 `d3d11.h`, `ID3D11`, `HLSL` 코드가 없음
- [ ] CI의 레이어 검사 통과
- [ ] 예측 질문 1, 2, 3을 실제로 해 보고 오류 메시지를 기록한 뒤 되돌림

## 기록

- [ ] SPEC 커밋 → 구현 커밋 순서
- [ ] `LOG.md` 1~2건

## 데모에서 설명할 수 있어야 하는 것

- 세 덩어리의 의존 방향과, 그 방향이 반대가 되면 생기는 문제
- `Renderer::Impl`이 하는 일. `Renderer.h`만 보고 Direct3D를 쓰는지 알 수 있는가?
- Camera는 왜 Engine에 있고, Render에는 없는가?
- GameObject를 만들지 않은 이유 (CubeViewer와 Sokoban 둘 다 쓰는가?)
