#!/usr/bin/env python3
"""자기 점검 보고서: 정답 공개 전에 내 과제가 어디까지 됐는지 단계별로 보여 준다.

    python tools/self_check.py --build out/build/x64-Debug                 (로컬)
    python tools/self_check.py --build build --screens screens --summary "$GITHUB_STEP_SUMMARY"   (CI)

확인하는 것 (CI가 볼 수 있는 것만. 최종 판단은 CHECKLIST.md와 데모)
  테스트   : 00 TextSokobanTests, 13 AStarTests의 통과 수와 내 시나리오 개수
  출력     : 13 maze.txt 경로 길이, 탐색 칸 수가 expected/tests.txt와 같은지
  화면     : screens/의 PNG가 steps/NN/expected/의 PNG와 같은지 (WARP 캡처끼리 비교)
  기록     : SPEC.md, LOG.md(거절/수정 기록), 13의 REVIEW.md(버그 3개)
  순서     : SPEC.md 커밋이 내 starter 수정보다 먼저인지 (git 기록)

보고서는 점검용이라 항상 종료 코드 0으로 끝난다.
"""
from __future__ import annotations

import argparse
import re
import struct
import subprocess
import sys
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

OK, WARN, FAIL, TODO, NA = "✅", "⚠️", "❌", "⬜", "—"

# 화면 비교: WARP 캡처는 PC와 Debug/Release에 관계없이 픽셀 단위로 같다 (확인함).
# 그래서 한 픽셀이라도 다르면 표시한다. 차이가 작으면(RATIO_SMALL 이하) HUD 글자 같은 작은 차이일 가능성이 높아 ⚠️로 둔다.
PIXEL_TOL = 0
RATIO_SMALL = 0.001


# ---------------------------------------------------------------------------
# PNG (외부 라이브러리 없이 읽기: 8비트 RGB/RGBA만)
# ---------------------------------------------------------------------------

def read_png(path: Path) -> tuple[int, int, int, bytes]:
    data = path.read_bytes()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError("PNG 아님")
    pos, idat = 8, b""
    width = height = channels = 0
    while pos < len(data):
        length, kind = struct.unpack(">I4s", data[pos:pos + 8])
        body = data[pos + 8:pos + 8 + length]
        pos += 12 + length
        if kind == b"IHDR":
            width, height, depth, color = struct.unpack(">IIBB", body[:10])
            if depth != 8 or color not in (2, 6) or body[12] != 0:
                raise ValueError(f"지원하지 않는 PNG 형식 (depth {depth}, color {color})")
            channels = 3 if color == 2 else 4
        elif kind == b"IDAT":
            idat += body
        elif kind == b"IEND":
            break
    raw = zlib.decompress(idat)
    stride = width * channels
    out = bytearray(height * stride)
    prev = bytearray(stride)
    i = 0
    for y in range(height):
        ftype = raw[i]
        line = bytearray(raw[i + 1:i + 1 + stride])
        i += 1 + stride
        for x in range(stride):
            a = line[x - channels] if x >= channels else 0
            b = prev[x]
            c = prev[x - channels] if x >= channels else 0
            if ftype == 1:
                line[x] = (line[x] + a) & 0xFF
            elif ftype == 2:
                line[x] = (line[x] + b) & 0xFF
            elif ftype == 3:
                line[x] = (line[x] + ((a + b) >> 1)) & 0xFF
            elif ftype == 4:
                p = a + b - c
                pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
                pred = a if pa <= pb and pa <= pc else (b if pb <= pc else c)
                line[x] = (line[x] + pred) & 0xFF
        out[y * stride:(y + 1) * stride] = line
        prev = line
    return width, height, channels, bytes(out)


def compare_png(mine: Path, expected: Path) -> tuple[str, str]:
    w1, h1, c1, p1 = read_png(mine)
    w2, h2, c2, p2 = read_png(expected)
    if (w1, h1) != (w2, h2):
        return FAIL, f"크기가 다름 ({w1}x{h1}, 기대 {w2}x{h2})"
    diff = 0
    for k in range(w1 * h1):
        a = p1[k * c1:k * c1 + 3]
        b = p2[k * c2:k * c2 + 3]
        if a != b and max(abs(a[0] - b[0]), abs(a[1] - b[1]), abs(a[2] - b[2])) > PIXEL_TOL:
            diff += 1
    if diff == 0:
        return OK, "기대 화면과 같음"
    ratio = diff / (w1 * h1)
    if ratio <= RATIO_SMALL:
        return WARN, f"거의 같지만 {diff}픽셀이 다름. HUD 숫자나 작은 표시를 기대 화면과 비교해 보세요"
    return FAIL, f"다름 ({ratio * 100:.1f}% 픽셀). 내 캡처와 기대 화면을 나란히 비교해 보세요"


# ---------------------------------------------------------------------------
# 도우미
# ---------------------------------------------------------------------------

def find_exe(build: Path | None, name: str, config: str | None) -> Path | None:
    if build is None or not build.is_dir():
        return None
    hits = [p for p in build.rglob(name + ("." + "exe" if sys.platform == "win32" else ""))
            if p.is_file() and "_deps" not in p.parts]
    if config:
        preferred = [p for p in hits if config.lower() in (q.lower() for q in p.parts)]
        hits = preferred or hits
    # 학생 레포(steps/)의 실행 파일을 먼저
    hits.sort(key=lambda p: (0 if "steps" in p.parts else 1, len(str(p))))
    return hits[0] if hits else None


def run(exe: Path, *args: str) -> str:
    # 절대 경로로 실행한다. cwd를 바꾸면 Linux는 상대 경로를 새 cwd 기준으로 찾아서 실행 파일을 못 찾는다 (Windows는 찾음)
    exe = exe.resolve()
    try:
        proc = subprocess.run([str(exe), *args], cwd=exe.parent, capture_output=True, timeout=60)
    except (OSError, subprocess.TimeoutExpired) as e:
        return f"[실행 실패] {e}"
    return proc.stdout.decode("utf-8", errors="replace").replace("\r\n", "\n")


def git(*args: str) -> str:
    try:
        return subprocess.run(["git", *args], cwd=ROOT, capture_output=True, timeout=30,
                              check=True).stdout.decode("utf-8", errors="replace")
    except (OSError, subprocess.SubprocessError):
        return ""


def my_scenarios(tests_cpp: Path) -> int:
    if not tests_cpp.exists():
        return 0
    text = tests_cpp.read_text(encoding="utf-8", errors="replace")
    found = re.search(r"-{2,}\s*MY SCENARIOS", text)  # 파일 머리 주석의 'MY SCENARIOS'가 아니라 표시 줄
    if found is None:
        return 0
    mark = found.start()
    end = text.find("};", mark)
    region = "\n".join(line for line in text[mark:end].splitlines() if not line.strip().startswith("//"))
    return region.count('{"')


def pass_count(output: str) -> tuple[int, int] | None:
    m = re.search(r"(\d+)\s*/\s*(\d+)\s*통과", output)
    return (int(m.group(1)), int(m.group(2))) if m else None


def filled(text: str, label: str) -> bool:
    """'- 라벨: 내용' 줄에 템플릿 안내문이 아닌 내용이 있는가"""
    for line in text.splitlines():
        if label in line and ":" in line:
            value = line.split(":", 1)[1].strip()
            if value and not value.startswith("("):
                return True
    return False


# ---------------------------------------------------------------------------
# 점검 항목
# ---------------------------------------------------------------------------

class Report:
    def __init__(self, step: Path):
        self.step = step
        self.tag = step.name.split("_", 1)[0]
        self.rows: list[tuple[str, str, str]] = []  # (상태, 항목, 설명)

    def add(self, state: str, item: str, note: str = "") -> None:
        self.rows.append((state, item, note))

    def started(self) -> bool:
        return any((self.step / f).exists() for f in ("SPEC.md", "LOG.md", "REVIEW.md"))

    def summary(self, kinds: tuple[str, ...]) -> str:
        states = [s for s, item, _ in self.rows if item.split(" ", 1)[0] in kinds]
        if not states:
            return NA
        for s in (FAIL, WARN, TODO):
            if s in states:
                return s
        return OK


def check_records(r: Report) -> None:
    spec, log = r.step / "SPEC.md", r.step / "LOG.md"
    if not spec.exists():
        r.add(TODO, "기록 SPEC.md", "아직 없음. templates/SPEC.md를 복사해 채운다")
    else:
        text = spec.read_text(encoding="utf-8", errors="replace")
        r.add(WARN if "NN_단계이름" in text else OK, "기록 SPEC.md",
              "제목이 템플릿 그대로" if "NN_단계이름" in text else "있음")
    if not log.exists():
        r.add(TODO, "기록 LOG.md", "아직 없음. templates/LOG.md 양식으로 1~2건")
    else:
        text = log.read_text(encoding="utf-8", errors="replace")
        entries = len(re.findall(r"^##\s+\d", text, re.M))
        judged = filled(text, "채택 / 수정 / 거절") and re.search(r"채택 / 수정 / 거절:\s*[^(\n]*(거절|수정)", text)
        quoted = filled(text, "내가 보낸 질문") or filled(text, "AI 답의 핵심")
        if entries == 0:
            r.add(WARN, "기록 LOG.md", "'## 1' 형식의 기록이 없음")
        elif not judged:
            r.add(WARN, "기록 LOG.md", f"{entries}건. 거절하거나 고쳐 쓴 기록이 아직 없음 (단계마다 1건 이상)")
        elif not quoted:
            r.add(WARN, "기록 LOG.md", f"{entries}건. 질문이나 AI 답의 원문이 비어 있음")
        else:
            r.add(OK, "기록 LOG.md", f"{entries}건, 거절/수정 기록 있음")


def check_order(r: Report) -> None:
    spec_rel = f"steps/{r.step.name}/SPEC.md"
    first = git("log", "--diff-filter=A", "--format=%H%x09%ae%x09%ct", "--", spec_rel).strip().splitlines()
    if not first:
        if (r.step / "SPEC.md").exists():
            r.add(WARN, "순서 SPEC 먼저", "SPEC.md가 아직 커밋되지 않음")
        return
    sha, author, _ = first[-1].split("\t")
    # SPEC 커밋의 조상 중, 같은 사람이 starter를 고친 커밋 (원본 업데이트 커밋은 작성자가 달라서 빠진다)
    earlier = git("log", "--no-merges", f"--author=<{author}>", "--format=%h %s", f"{sha}^",
                  "--", f"steps/{r.step.name}/starter").strip().splitlines()
    # 원본 업데이트 커밋은 steps/ 밖(docs/, templates/ 등)도 함께 바꾼다. 내 작업 커밋은 steps/ 안만 바꾼다.
    # (교수자가 같은 git 이메일로 테스트할 때 원본 커밋이 섞이는 것을 막는다)
    earlier = [line for line in earlier
               if all(f.startswith("steps/") for f in git("show", "--name-only", "--format=", line.split(" ", 1)[0]).split())]
    if earlier:
        r.add(FAIL, "순서 SPEC 먼저", f"SPEC 커밋({sha[:7]}) 전에 내가 starter를 고친 커밋이 있음: {earlier[-1]}")
    else:
        r.add(OK, "순서 SPEC 먼저", f"SPEC 커밋 {sha[:7]}이 starter 수정보다 먼저")


def check_tests(r: Report, build: Path | None, config: str | None) -> None:
    exe_name = {"00": "TextSokobanTests", "13": "AStarTests"}.get(r.tag)
    if exe_name is None:
        return
    mine = my_scenarios(r.step / "starter" / "tests.cpp")
    r.add(OK if mine >= 2 else TODO, "테스트 내 시나리오", f"{mine}개 (2개 이상)")
    exe = find_exe(build, exe_name, config)
    if exe is None:
        r.add(NA, "테스트 실행", f"{exe_name}를 찾지 못함 (빌드 폴더를 --build로 지정)")
        return
    out = run(exe)
    counts = pass_count(out)
    if counts is None:
        r.add(FAIL, "테스트 실행", "결과를 읽지 못함 (실행 오류?)")
        return
    ok, total = counts
    failed = [line[7:] for line in out.splitlines() if line.startswith("[FAIL]")]
    r.add(OK if ok == total else FAIL, "테스트 통과", f"{ok} / {total}" + (f" (실패: {', '.join(failed[:3])})" if failed else ""))

    expected = r.step / "expected" / "tests.txt"
    if r.tag == "13" and expected.exists() and ok == total:
        want_text = expected.read_text(encoding="utf-8-sig").replace("\r\n", "\n")
        want = dict(re.findall(r"^\[PASS\] (.+?)  \((.+?)\)$", want_text, re.M))
        got = dict(re.findall(r"^\[PASS\] (.+?)  \((.+?)\)$", out, re.M))
        differ = [name for name, v in want.items() if got.get(name) != v]
        if differ:
            name = differ[0]
            r.add(WARN, "출력 탐색 칸", f"{len(differ)}개 시나리오가 expected와 다름. 예: {name} 내 결과 '{got.get(name)}', 기대 '{want[name]}'"
                  " (최단이면 PASS지만, 탐색 칸 차이는 데모에서 설명할 수 있어야 함)")
        else:
            r.add(OK, "출력 탐색 칸", "제공 시나리오의 길이와 탐색 칸이 expected/tests.txt와 같음")

    if r.tag == "13":
        console = find_exe(build, "AStarConsole", config)
        if console:
            out = run(console, "maps/maze.txt")
            m = re.search(r"경로 길이\(이동 횟수\): (\d+)", out)
            if m:
                r.add(OK if m.group(1) == "20" else FAIL, "출력 maze.txt", f"경로 길이 {m.group(1)} (기대 20)")
            else:
                r.add(FAIL, "출력 maze.txt", "경로 없음 또는 실행 오류")


def check_review(r: Report) -> None:
    if r.tag != "13":
        return
    review = r.step / "REVIEW.md"
    if not review.exists():
        r.add(TODO, "기록 REVIEW.md", "AI 검증 과제: 버그 3개의 증상/원인/증거")
        return
    text = review.read_text(encoding="utf-8", errors="replace")
    blocks = re.split(r"^\s*#*\s*버그\s*\d+", text, flags=re.M)[1:]
    complete = sum(1 for b in blocks if filled(b, "원인") and filled(b, "증거"))
    r.add(OK if complete >= 3 else WARN, "기록 REVIEW.md", f"원인과 증거가 채워진 버그 {complete} / 3")


def check_screens(r: Report, screens: Path | None) -> None:
    pngs = sorted((r.step / "expected").glob("*.png"))
    if not pngs:
        return
    for exp in pngs:
        if screens is None:
            r.add(NA, f"화면 {exp.name}", "제출 태그 CI(windows)에서 비교")
            continue
        mine = screens / exp.name
        if not mine.exists():
            r.add(FAIL, f"화면 {exp.name}", "캡처가 없음 (빌드나 실행 실패)")
            continue
        try:
            state, note = compare_png(mine, exp)
        except (ValueError, zlib.error) as e:
            r.add(FAIL, f"화면 {exp.name}", f"PNG를 읽지 못함: {e}")
            continue
        r.add(state, f"화면 {exp.name}", note)


# ---------------------------------------------------------------------------

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--build", type=Path, help="빌드 폴더 (예: out/build/x64-Debug, build)")
    parser.add_argument("--config", help="여러 구성이 있을 때 고를 구성 (Debug, Release)")
    parser.add_argument("--screens", type=Path, help="capture_steps.ps1이 만든 PNG 폴더")
    parser.add_argument("--steps", nargs="*", help="이 단계만 (예: 13 14). 기본: 시작한 단계 + 테스트/화면이 있는 단계")
    parser.add_argument("--summary", type=Path, help="보고서를 이 파일 끝에 덧붙인다 (CI: $GITHUB_STEP_SUMMARY)")
    args = parser.parse_args()

    reports = []
    for step in sorted((ROOT / "steps").glob("[0-9][0-9]_*")):
        r = Report(step)
        if args.steps and r.tag not in args.steps:
            continue
        check_tests(r, args.build, args.config)
        check_screens(r, args.screens)
        check_records(r)
        check_review(r)
        if r.started():
            check_order(r)
        reports.append(r)

    def shown(r: Report) -> bool:
        if args.steps or r.started():
            return True
        return any(s == OK for s, item, _ in r.rows if item.startswith(("테스트 통과", "화면")))

    lines = ["## 자기 점검 보고서", "",
             "정답 공개 전에 내 과제 상태를 보는 표입니다. ✅ 확인됨 · ⚠️ 확인 필요 · ❌ 기대와 다름 · ⬜ 아직 안 함 · — 여기서는 확인 불가",
             "CI가 볼 수 없는 것(동작, 설명, 데모)은 각 단계 `CHECKLIST.md`로 확인합니다.", "",
             "| 단계 | 테스트·출력 | 화면 | 기록 | 순서 |", "|---|---|---|---|---|"]
    visible = [r for r in reports if shown(r)]
    for r in visible:
        lines.append(f"| {r.step.name} | {r.summary(('테스트', '출력'))} | {r.summary(('화면',))} | "
                     f"{r.summary(('기록',))} | {r.summary(('순서',))} |")
    if not visible:
        lines.append("| (시작한 단계 없음) | | | | |")
    for r in visible:
        lines += ["", f"### {r.step.name}", "", "| | 항목 | 내용 |", "|---|---|---|"]
        lines += [f"| {s} | {item} | {note.replace('|', '/')} |" for s, item, note in r.rows]
    hidden = [r.step.name for r in reports if not shown(r)]
    if hidden:
        lines += ["", f"<details><summary>시작하지 않은 단계 {len(hidden)}개</summary>", "", ", ".join(hidden), "", "</details>"]
    text = "\n".join(lines) + "\n"

    print(text)
    if args.summary:
        with open(args.summary, "a", encoding="utf-8") as f:
            f.write(text)
    return 0


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.exit(main())
