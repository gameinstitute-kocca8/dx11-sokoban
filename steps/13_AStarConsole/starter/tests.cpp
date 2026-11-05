// tests.cpp - A* 시나리오 테스트
//
// 경로의 '모양'은 검사하지 않는다. 길이가 같은 최단 경로가 여러 개일 수 있고, 어느 것을 고를지는
// open 목록에서 동점을 어떻게 깨느냐에 달려 있기 때문이다. 대신 다음을 검사한다.
//   1) 찾았는지 여부가 맞는가
//   2) 경로가 올바른가: start로 시작해 goal로 끝나고, 한 칸씩 상하좌우로만 움직이며, 막힌 칸을 지나지 않는다
//   3) 길이가 최단인가: 너비 우선 탐색(BFS)으로 구한 최단 거리와 같은가
// 학생은 맨 아래 MY SCENARIOS에 직접 만든 시나리오를 2개 이상 추가한다.
#include "AStar.h"

#include <cstdlib>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace astar;

namespace {

struct Scenario {
    const char* name;
    const char* map;  // # 벽, . 빈칸, S 시작, G 목표
};

// clang-format off
const std::vector<Scenario> kScenarios = {
    {"직선",                 "S...G\n"},
    {"시작이 곧 목표",        "...\n.S.\n...\n"},  // G가 없으므로 아래 Run에서 따로 처리
    {"벽 하나 돌아가기",      "S#G\n...\n"},
    {"막다른 길 무시",        "S...#\n.##.#\n.#G.#\n.####\n"},
    {"길 없음",              "S.#.\n..#G\n..#.\n"},
    {"목표가 벽 안",          ".....\n.S###\n...#G\n...##\n"},
    {"넓은 빈 맵",           "S.........\n..........\n..........\n.........G\n"},
    {"ㄷ자 벽 (휴리스틱 함정)", "..........\n.####.....\n....#.....\n.S..#...G.\n....#.....\n.####.....\n..........\n"},

    // ---- MY SCENARIOS: 아래에 직접 만든 시나리오를 2개 이상 추가한다 ----

};
// clang-format on

// 비교용: 너비 우선 탐색으로 구한 최단 거리. 길이 없으면 -1
int BfsDistance(const Grid& grid, Int2 start, Int2 goal) {
    if (!grid.Passable(start) || !grid.Passable(goal)) return -1;
    std::vector<int> dist(grid.width * grid.height, -1);
    std::queue<Int2> q;
    dist[grid.Index(start)] = 0;
    q.push(start);
    const Int2 dirs[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    while (!q.empty()) {
        const Int2 c = q.front();
        q.pop();
        if (c == goal) return dist[grid.Index(c)];
        for (const Int2& d : dirs) {
            const Int2 n{c.x + d.x, c.y + d.y};
            if (grid.Passable(n) && dist[grid.Index(n)] < 0) {
                dist[grid.Index(n)] = dist[grid.Index(c)] + 1;
                q.push(n);
            }
        }
    }
    return -1;
}

// 경로가 규칙에 맞는지. 문제가 있으면 이유를 돌려준다
std::string CheckPath(const Grid& grid, Int2 start, Int2 goal, const std::vector<Int2>& path) {
    if (path.empty()) return "경로가 비어 있음";
    if (path.front() != start) return "경로가 start에서 시작하지 않음";
    if (path.back() != goal) return "경로가 goal에서 끝나지 않음";
    for (size_t i = 0; i < path.size(); ++i) {
        if (!grid.Passable(path[i])) return "막힌 칸을 지나감 (" + std::to_string(i) + "번째)";
        if (i > 0) {
            const int step = std::abs(path[i].x - path[i - 1].x) + std::abs(path[i].y - path[i - 1].y);
            if (step != 1) return "한 칸씩 상하좌우로 움직이지 않음 (" + std::to_string(i) + "번째)";
        }
    }
    return "";
}

bool Run(const Scenario& s) {
    Grid grid;
    Int2 start;
    Int2 goal;
    std::string error;
    std::string map = s.map;
    const bool sameCell = map.find('G') == std::string::npos;
    if (sameCell) map[map.find('S') + 1] = 'G';  // 형식을 맞추려고 G를 옆에 두고, 아래에서 goal = start로 바꾼다
    if (!LoadGridFromText(map, grid, start, goal, error)) {
        std::cout << "[ERROR] " << s.name << ": " << error << '\n';
        return false;
    }
    if (sameCell) goal = start;

    const Result result = FindPath(grid, start, goal);
    const int expected = BfsDistance(grid, start, goal);

    std::string problem;
    if (expected < 0) {
        if (result.found) problem = "길이 없는데 찾았다고 함";
        else if (!result.path.empty()) problem = "길이 없는데 경로가 비어 있지 않음";
    } else {
        if (!result.found) {
            problem = "길이 있는데 못 찾음";
        } else {
            problem = CheckPath(grid, start, goal, result.path);
            const int length = static_cast<int>(result.path.size()) - 1;
            if (problem.empty() && length != expected) {
                problem = "최단이 아님: 길이 " + std::to_string(length) + ", 최단 " + std::to_string(expected);
            }
        }
    }

    std::cout << (problem.empty() ? "[PASS] " : "[FAIL] ") << s.name;
    if (expected >= 0 && result.found) std::cout << "  (길이 " << result.path.size() - 1 << ", 탐색 " << result.expanded.size() << "칸)";
    std::cout << '\n';
    if (!problem.empty()) {
        std::cout << "  " << problem << '\n' << ToText(grid, start, goal, result, true);
    }
    return problem.empty();
}

bool HeuristicTest() {
    const bool ok = Heuristic({0, 0}, {3, 4}) == 7 && Heuristic({5, 2}, {1, 2}) == 4 && Heuristic({2, 2}, {2, 2}) == 0;
    std::cout << (ok ? "[PASS] " : "[FAIL] ") << "휴리스틱: 맨해튼 거리\n";
    return ok;
}

}  // namespace

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    int failed = HeuristicTest() ? 0 : 1;
    for (const Scenario& s : kScenarios) {
        if (!Run(s)) ++failed;
    }
    const size_t total = kScenarios.size() + 1;
    std::cout << '\n' << (total - failed) << " / " << total << " 통과\n";
    return failed == 0 ? 0 : 1;
}
