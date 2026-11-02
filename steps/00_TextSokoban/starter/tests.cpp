// tests.cpp - 규칙 시나리오 테스트
// 각 시나리오: 시작 레벨 + 입력 → 기대 결과(각 입력의 MoveResult) + 기대 최종 맵 + Clear 여부
// 이 파일이 00단계의 '기대 화면'이다. 학생은 아래 MY SCENARIOS에 직접 시나리오 2개 이상을 추가한다.
#include "Sokoban.h"

#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace sokoban;

namespace {

struct Scenario {
    const char* name;
    const char* start;     // 시작 맵
    const char* input;     // w a s d
    const char* results;   // 입력마다 한 글자: M(Moved) P(Pushed) B(Blocked)
    const char* expected;  // 기대 최종 맵
    bool cleared;
};

// clang-format off
const std::vector<Scenario> kScenarios = {
    {"빈 칸으로 이동",
     "#####\n#@  #\n#####\n", "d", "M",
     "#####\n# @ #\n#####\n", false},

    {"벽으로는 이동 불가",
     "###\n#@#\n###\n", "wasd", "BBBB",
     "###\n#@#\n###\n", false},

    {"상자 한 칸 밀기",
     "######\n#@$  #\n######\n", "d", "P",
     "######\n# @$ #\n######\n", false},

    {"상자 뒤가 벽이면 밀 수 없음",
     "#####\n# @$#\n#####\n", "d", "B",
     "#####\n# @$#\n#####\n", false},

    {"상자 두 개는 밀 수 없음",
     "######\n#@$$ #\n######\n", "d", "B",
     "######\n#@$$ #\n######\n", false},

    {"상자를 목표에 올리면 Clear",
     "#####\n#@$.#\n#####\n", "d", "P",
     "#####\n# @*#\n#####\n", true},

    {"목표 위의 상자를 다시 밀어내면 Clear 해제",
     "######\n#@*  #\n######\n", "d", "P",
     "######\n# +$ #\n######\n", false},

    {"목표 위의 플레이어가 떠나면 목표가 남음",
     "#####\n#+  #\n#####\n", "d", "M",
     "#####\n#.@ #\n#####\n", false},

    {"세로 방향 밀기",
     "###\n#@#\n#$#\n#.#\n###\n", "s", "P",
     "###\n# #\n#@#\n#*#\n###\n", true},

    {"목표 두 개 중 하나만 채우면 Clear 아님",
     "#######\n#.$@$.#\n#######\n", "a", "P",
     "#######\n#*@ $.#\n#######\n", false},

    {"맵 밖은 벽으로 취급",
     "@$\n", "da", "BB",
     "@$\n", false},

    // ---- MY SCENARIOS: 아래에 직접 만든 시나리오를 2개 이상 추가한다 ----

};
// clang-format on

char ResultChar(MoveResult r) {
    switch (r) {
        case MoveResult::Moved: return 'M';
        case MoveResult::Pushed: return 'P';
        case MoveResult::Blocked: return 'B';
    }
    return '?';
}

Dir CharToDir(char c) {
    switch (c) {
        case 'w': return Dir::Up;
        case 's': return Dir::Down;
        case 'a': return Dir::Left;
        default: return Dir::Right;
    }
}

bool Run(const Scenario& s) {
    Level level;
    std::string error;
    if (!LoadLevelFromText(s.start, level, error)) {
        std::cout << "[ERROR] " << s.name << ": 시작 맵 오류 - " << error << '\n';
        return false;
    }
    std::string results;
    for (const char* c = s.input; *c; ++c) results += ResultChar(TryMove(level, CharToDir(*c)));

    const std::string finalMap = ToText(level);
    const bool cleared = IsCleared(level);
    const bool ok = results == s.results && finalMap == s.expected && cleared == s.cleared;

    std::cout << (ok ? "[PASS] " : "[FAIL] ") << s.name << '\n';
    if (!ok) {
        std::cout << "  입력: " << s.input << '\n';
        std::cout << "  결과  기대=" << s.results << "  실제=" << results << '\n';
        std::cout << "  Clear 기대=" << (s.cleared ? "yes" : "no") << "  실제=" << (cleared ? "yes" : "no") << '\n';
        std::cout << "  기대 맵:\n" << s.expected << "  실제 맵:\n" << finalMap;
    }
    return ok;
}

}  // namespace

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    int failed = 0;
    for (const Scenario& s : kScenarios) {
        if (!Run(s)) ++failed;
    }
    std::cout << '\n' << (kScenarios.size() - failed) << " / " << kScenarios.size() << " 통과\n";
    return failed == 0 ? 0 : 1;
}
