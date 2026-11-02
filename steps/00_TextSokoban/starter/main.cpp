// main.cpp - 콘솔 소코반
// 사용법:
//   TextSokoban                      levels/level01.txt로 플레이
//   TextSokoban <레벨 파일>          지정한 레벨로 플레이
//   TextSokoban <레벨 파일> <입력>   입력(예: "ddwwa")을 실행하고 최종 상태만 출력 (검증용)
//
// 플레이 중 입력: w a s d 이동, r 재시작, q 종료. 여러 글자를 한 줄에 입력해도 된다.
#include "Sokoban.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace sokoban;

namespace {

bool ReadFile(const std::string& path, std::string& out) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;
    std::ostringstream ss;
    ss << file.rdbuf();
    out = ss.str();
    return true;
}

const char* ResultName(MoveResult r) {
    switch (r) {
        case MoveResult::Moved: return "Moved";
        case MoveResult::Pushed: return "Pushed";
        case MoveResult::Blocked: return "Blocked";
    }
    return "?";
}

// 입력 한 글자를 방향으로 바꾼다. 방향 키가 아니면 false.
bool CharToDir(char c, Dir& dir) {
    switch (c) {
        case 'w': case 'W': dir = Dir::Up; return true;
        case 's': case 'S': dir = Dir::Down; return true;
        case 'a': case 'A': dir = Dir::Left; return true;
        case 'd': case 'D': dir = Dir::Right; return true;
        default: return false;
    }
}

// 검증 모드: 입력 문자열을 실행하고, 각 결과와 최종 상태를 출력한다.
int RunScript(Level level, const std::string& input) {
    int moves = 0;
    for (char c : input) {
        Dir dir;
        if (!CharToDir(c, dir)) continue;
        const MoveResult r = TryMove(level, dir);
        if (r != MoveResult::Blocked) ++moves;
        std::cout << c << ' ' << ResultName(r) << '\n';
    }
    std::cout << ToText(level);
    std::cout << "moves=" << moves << " cleared=" << (IsCleared(level) ? "yes" : "no") << '\n';
    return 0;
}

int RunInteractive(const Level& initial) {
    Level level = initial;
    int moves = 0;
    while (true) {
        std::cout << '\n' << ToText(level) << "이동 " << moves << "회  (wasd 이동, r 재시작, q 종료) > ";
        if (IsCleared(level)) {
            std::cout << "\nCLEAR! 총 " << moves << "회 이동\n";
            return 0;
        }
        std::string line;
        if (!std::getline(std::cin, line)) return 0;
        for (char c : line) {
            if (c == 'q' || c == 'Q') return 0;
            if (c == 'r' || c == 'R') {
                level = initial;
                moves = 0;
                continue;
            }
            Dir dir;
            if (!CharToDir(c, dir)) continue;
            if (TryMove(level, dir) != MoveResult::Blocked) ++moves;
            if (IsCleared(level)) break;
        }
    }
}

}  // namespace

int main(int argc, char** argv) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    const std::string path = argc >= 2 ? argv[1] : "levels/level01.txt";
    std::string text;
    if (!ReadFile(path, text)) {
        std::cerr << "레벨 파일을 열 수 없음: " << path << '\n';
        return 1;
    }
    Level level;
    std::string error;
    if (!LoadLevelFromText(text, level, error)) {
        std::cerr << "레벨 오류 (" << path << "): " << error << '\n';
        return 1;
    }
    if (argc >= 3) return RunScript(level, argv[2]);
    return RunInteractive(level);
}
