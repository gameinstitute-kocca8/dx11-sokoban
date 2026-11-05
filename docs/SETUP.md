# 개발 환경 준비

## 설치

1. **Visual Studio 2022 이상** (Community 가능)
   - 워크로드: **C++를 사용한 데스크톱 개발**
   - 개별 구성 요소에서 다음이 체크되어 있는지 확인합니다: **Windows 11 SDK**(최신), **Windows용 C++ CMake 도구**
2. **Git**과 GitHub 계정
3. **D3D11 디버그 레이어** (권장)
   - Windows 설정 → 시스템 → 선택적 기능 → 기능 추가 → **그래픽 도구**
   - 설치하지 않아도 실행은 됩니다. 다만 Debug 빌드에서 "디버그 레이어 없이 다시 시도합니다"라는 로그가 나오고, D3D 경고 메시지를 볼 수 없습니다.
4. **RenderDoc** (권장): <https://renderdoc.org>

## 빌드와 실행 (Visual Studio)

1. **파일 → 열기 → 폴더**로 레포 폴더를 엽니다. CMake 구성이 자동으로 시작됩니다.
   - **빌드 폴더마다 처음 구성할 때** DirectXTK를 내려받아 빌드하므로 몇 분 걸립니다. 인터넷 연결이 필요합니다.
   - 받은 DirectXTK는 빌드 폴더 안(`_deps/`)에만 들어갑니다. PC에 다른 버전이 설치되어 있어도 서로 영향이 없습니다.
2. 위쪽 도구 모음의 **시작 항목**에서 실행 파일을 고릅니다. 예: `TextSokobanTests.exe`, `01_ClearColor.exe`
3. **F5**로 실행합니다. 실행 중 로그는 **출력 창**(보기 → 출력)에 나옵니다.

`.sln` 파일이 편하다면 개발자 명령 프롬프트에서 다음을 실행해 `build/` 아래에 솔루션을 만들 수 있습니다.

```bat
cmake -S . -B build -A x64
```

## 명령줄 빌드

```bat
cmake -S . -B build -A x64
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

## 화면 캡처 (CI와 같은 방법)

모든 DX11 단계 실행 파일은 다음 옵션을 지원합니다.

```bat
01_ClearColor.exe --capture out.png            REM WARP로 한 프레임 그리고 저장 후 종료
04_Transform.exe  --capture out.png --time 1.0  REM 1초 시점의 화면
```

WARP는 GPU 없이 CPU로 그리는 Direct3D 소프트웨어 렌더러입니다. 그래서 어느 PC에서나 같은 결과가 나오고, 이 결과를 `expected/`의 기대 화면과 비교합니다.

모든 단계를 한 번에 캡처하려면 다음을 실행합니다.

```powershell
pwsh tools/capture_steps.ps1 -BuildDir build -OutDir screens
```

## 자주 만나는 문제

| 증상 | 확인할 것 |
|---|---|
| 한글 로그가 깨짐 | 소스 파일이 UTF-8인지, CMake가 `/utf-8` 옵션을 넣는지 (`cmake/Course.cmake`) |
| "셰이더 컴파일 실패" | 출력 창의 `[셰이더]` 줄에 파일명(줄, 칸)과 오류 내용이 있습니다 |
| 실행하자마자 "초기화 실패" 창 | 출력 창에서 `[실패]`로 시작하는 줄을 찾습니다 |
| 화면이 01의 배경색뿐 | RenderDoc으로 Draw 호출이 있는지, 정점이 어디에 있는지 확인합니다 |
| 구성 단계에서 DirectXTK 다운로드 실패 | 인터넷 연결 또는 회사/학교 프록시 설정 |
