#!/usr/bin/env python3
"""레이어 의존 방향 검사:  Game → Engine → Render

    python tools/check_layers.py [검사할 폴더 ...]   (기본: 현재 폴더)

경로에 Render/, Engine/, Game/ 폴더가 들어 있는 소스 파일만 검사한다.
07단계에서 폴더를 나누기 전까지는 '검사 대상 없음'이 정상이다.

규칙
  Render : Engine/, Game/ 헤더를 include하지 않는다. 게임 단어(Sokoban, Crate, Goal, Player)를 쓰지 않는다.
  Engine : Game/ 헤더를 include하지 않는다. 게임 단어를 쓰지 않는다.
  Game   : Direct3D/DXGI/셰이더 컴파일러/텍스처 로더 헤더를 직접 include하지 않는다.
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

SOURCE_EXT = {".cpp", ".h", ".hpp", ".inl"}
INCLUDE_RE = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]')
GAME_WORDS_RE = re.compile(r"\b(Sokoban|Crate|Goal|Player)\b")
D3D_HEADERS = {
    "d3d11.h", "d3d11_1.h", "d3d11_2.h", "dxgi.h", "dxgi1_2.h", "d3dcompiler.h",
    "WICTextureLoader.h", "DDSTextureLoader.h",
}
SKIP_DIRS = {"build", "out", ".vs", ".git", "_deps"}


def layer_of(path: Path) -> str | None:
    for part in reversed(path.parts):
        if part in ("Render", "Engine", "Game"):
            return part
    return None


def strip_comments(line: str) -> str:
    return line.split("//", 1)[0]


def check_file(path: Path, layer: str) -> list[str]:
    problems: list[str] = []
    for number, line in enumerate(path.read_text(encoding="utf-8", errors="replace").splitlines(), 1):
        where = f"{path}:{number}"
        m = INCLUDE_RE.match(line)
        if m:
            target = m.group(1).replace("\\", "/")
            parts = set(target.split("/"))
            if layer == "Render" and ({"Engine", "Game"} & parts):
                problems.append(f"{where}: Render가 상위 레이어를 include함 → {target}")
            if layer == "Engine" and "Game" in parts:
                problems.append(f"{where}: Engine이 Game을 include함 → {target}")
            if layer == "Game" and target.split("/")[-1] in D3D_HEADERS:
                problems.append(f"{where}: Game이 렌더링 API 헤더를 직접 include함 → {target}")
            continue
        if layer in ("Render", "Engine"):
            word = GAME_WORDS_RE.search(strip_comments(line))
            if word:
                problems.append(f"{where}: {layer}에 게임 단어 '{word.group(1)}'가 있음")
    return problems


def main() -> int:
    roots = [Path(p) for p in sys.argv[1:]] or [Path(".")]
    checked = 0
    problems: list[str] = []
    for root in roots:
        for path in sorted(root.rglob("*")):
            if path.suffix.lower() not in SOURCE_EXT or SKIP_DIRS & set(path.parts):
                continue
            layer = layer_of(path.relative_to(root))
            if layer is None:
                continue
            checked += 1
            problems += check_file(path, layer)

    if checked == 0:
        print("레이어 검사: 검사 대상 없음 (Render/, Engine/, Game/ 폴더가 아직 없음)")
        return 0
    for p in problems:
        print(p)
    print(f"레이어 검사: 파일 {checked}개, 위반 {len(problems)}건")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
