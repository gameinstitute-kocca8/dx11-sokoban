// ai_review/AStar_ai.cpp - "AI가 짜 준" A* (검토용, 13단계 AI 검증 과제)
//
// 어떤 학생이 AI에게 "A* 짜 줘"라고 해서 받은 코드라고 가정한다. 컴파일되고, 대부분 그럴듯하게 동작한다.
// 하지만 버그가 3개 숨어 있다. AStarReviewTests.exe / AStarReviewConsole.exe가 이 파일로 빌드된다.
//
// 이 파일을 고쳐서 내 AStar.cpp에 붙여 넣지 않는다. 할 일은 TASK.md의 "AI 검증 과제"를 따른다.
#include "AStar.h"

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <queue>
#include <sstream>

namespace astar {

namespace {

struct OpenEntry {
    int f;
    int h;
    int index;
};

// 우선순위 큐 비교: f가 작은 것 먼저, f가 같으면 목표에 가까운(h가 작은) 것 먼저
struct WorseThan {
    bool operator()(const OpenEntry& a, const OpenEntry& b) const {
        if (a.f != b.f) return a.f > b.f;
        return a.h < b.h;
    }
};

const Int2 kDirections[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

Int2 FromIndex(const Grid& grid, int index) {
    return Int2{index % grid.width, index / grid.width};
}

std::vector<Int2> ReconstructPath(const Grid& grid, const std::vector<int>& cameFrom, int goalIndex) {
    std::vector<Int2> path;
    for (int i = goalIndex; i != -1; i = cameFrom[i]) {
        path.push_back(FromIndex(grid, i));
    }
    std::reverse(path.begin(), path.end());
    return path;
}

}  // namespace

int Heuristic(Int2 a, Int2 b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

Result FindPath(const Grid& grid, Int2 start, Int2 goal) {
    Result result;
    // 잘못된 입력과, 움직일 필요가 없는 경우는 바로 반환
    if (!grid.Passable(start) || !grid.Passable(goal) || start == goal) return result;

    const int count = grid.width * grid.height;
    std::vector<int> g(count, INT_MAX);
    std::vector<int> cameFrom(count, -1);
    std::vector<bool> closed(count, false);
    std::priority_queue<OpenEntry, std::vector<OpenEntry>, WorseThan> open;

    // 휴리스틱에 가중치를 주면 목표 쪽으로 더 빨리 수렴해서 탐색 칸 수가 줄어든다 (성능 최적화)
    constexpr int kHeuristicWeight = 2;

    const int startIndex = grid.Index(start);
    g[startIndex] = 0;
    open.push(OpenEntry{kHeuristicWeight * Heuristic(start, goal), Heuristic(start, goal), startIndex});

    while (!open.empty()) {
        const OpenEntry current = open.top();
        open.pop();
        if (closed[current.index]) continue;
        closed[current.index] = true;

        const Int2 cell = FromIndex(grid, current.index);
        result.expanded.push_back(cell);
        if (cell == goal) {
            result.found = true;
            result.path = ReconstructPath(grid, cameFrom, current.index);
            return result;
        }

        for (const Int2& d : kDirections) {
            const Int2 next{cell.x + d.x, cell.y + d.y};
            if (!grid.Passable(next)) continue;
            const int nextIndex = grid.Index(next);
            if (closed[nextIndex]) continue;
            const int tentative = g[current.index] + 1;
            if (tentative < g[nextIndex]) {
                g[nextIndex] = tentative;
                cameFrom[nextIndex] = current.index;
                const int h = Heuristic(next, goal);
                open.push(OpenEntry{tentative + kHeuristicWeight * h, h, nextIndex});
            }
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
// 맵 읽기/쓰기 (제공 코드)
// ---------------------------------------------------------------------------

bool LoadGridFromText(const std::string& text, Grid& grid, Int2& start, Int2& goal, std::string& error) {
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty() && line[0] == ';') continue;
        lines.push_back(line);
    }
    while (!lines.empty() && lines.back().find_first_not_of(' ') == std::string::npos) lines.pop_back();
    if (lines.empty()) {
        error = "빈 맵";
        return false;
    }

    Grid result;
    result.height = static_cast<int>(lines.size());
    for (const std::string& l : lines) result.width = std::max(result.width, static_cast<int>(l.size()));
    result.blocked.assign(result.width * result.height, false);

    int starts = 0;
    int goals = 0;
    for (int y = 0; y < result.height; ++y) {
        for (int x = 0; x < result.width; ++x) {
            const char c = x < static_cast<int>(lines[y].size()) ? lines[y][x] : ' ';
            switch (c) {
                case '#': result.blocked[result.Index({x, y})] = true; break;
                case '.':
                case ' ': break;
                case 'S': start = {x, y}; ++starts; break;
                case 'G': goal = {x, y}; ++goals; break;
                default:
                    error = std::string("알 수 없는 문자 '") + c + "'";
                    return false;
            }
        }
    }
    if (starts != 1 || goals != 1) {
        error = "S와 G는 정확히 하나씩이어야 함";
        return false;
    }
    grid = result;
    return true;
}

std::string ToText(const Grid& grid, Int2 start, Int2 goal, const Result& result, bool showExpanded) {
    std::vector<char> cells(grid.width * grid.height, '.');
    for (int i = 0; i < grid.width * grid.height; ++i) {
        if (grid.blocked[i]) cells[i] = '#';
    }
    if (showExpanded) {
        for (const Int2& p : result.expanded) cells[grid.Index(p)] = 'o';
    }
    for (const Int2& p : result.path) cells[grid.Index(p)] = '*';
    cells[grid.Index(start)] = 'S';
    cells[grid.Index(goal)] = 'G';

    std::string text;
    for (int y = 0; y < grid.height; ++y) {
        text.append(cells.begin() + y * grid.width, cells.begin() + (y + 1) * grid.width);
        text += '\n';
    }
    return text;
}

}  // namespace astar
