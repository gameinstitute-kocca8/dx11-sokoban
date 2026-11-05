// AStar.cpp
#include "AStar.h"

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <queue>
#include <sstream>

namespace astar {

namespace {

// 열린 목록(open)의 원소. f = g + h
struct OpenEntry {
    int f;
    int h;
    int index;  // grid.Index(칸)
};

// std::priority_queue는 '가장 큰 것'을 먼저 꺼낸다. 그래서 '더 나쁜 것'을 크다고 정의한다.
//   f가 작은 것이 먼저. f가 같으면 h가 작은 것(목표에 더 가까워 보이는 것)이 먼저.
struct WorseThan {
    bool operator()(const OpenEntry& a, const OpenEntry& b) const {
        if (a.f != b.f) return a.f > b.f;
        return a.h > b.h;
    }
};

const Int2 kDirections[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

Int2 FromIndex(const Grid& grid, int index) {
    return Int2{index % grid.width, index / grid.width};
}

// cameFrom을 goal에서 거꾸로 따라가 start → goal 순서의 경로를 만든다.
std::vector<Int2> ReconstructPath(const Grid& grid, const std::vector<int>& cameFrom, int goalIndex) {
    std::vector<Int2> path;
    // TODO(13): goal에서 시작해 cameFrom을 따라 -1이 나올 때까지 거슬러 올라간 뒤, 순서를 뒤집는다.
    //   힌트: start 칸의 cameFrom은 -1이다. std::reverse(path.begin(), path.end())
    (void)grid;
    (void)cameFrom;
    (void)goalIndex;
    return path;
}

}  // namespace

int Heuristic(Int2 a, Int2 b) {
    // TODO(13): 맨해튼 거리. 4방향 이동에서 실제 거리보다 절대 크지 않다(admissible) → A*가 최단 경로를 보장한다.
    (void)a;
    (void)b;
    return 0;
}

Result FindPath(const Grid& grid, Int2 start, Int2 goal) {
    Result result;
    if (!grid.Passable(start) || !grid.Passable(goal)) return result;

    const int count = grid.width * grid.height;
    std::vector<int> g(count, INT_MAX);      // start에서 이 칸까지 알려진 가장 짧은 거리
    std::vector<int> cameFrom(count, -1);    // 그 거리로 올 때 직전 칸
    std::vector<bool> closed(count, false);  // 이미 최단 거리가 확정된 칸
    std::priority_queue<OpenEntry, std::vector<OpenEntry>, WorseThan> open;

    // TODO(13): A* 본체.
    //   힌트: 1) start의 g = 0으로 open에 넣는다 (f = h)
    //   힌트: 2) open에서 f가 가장 작은 칸을 꺼낸다. 이미 closed면 건너뛴다 (같은 칸이 여러 번 들어갈 수 있다)
    //   힌트: 3) closed로 표시하고 result.expanded에 넣는다. goal이면 경로를 만들고 끝
    //   힌트: 4) 이웃 4칸: 갈 수 있고 closed가 아니며, g[현재] + 1이 g[이웃]보다 작으면 g, cameFrom 갱신 후 open에 넣는다
    //   힌트: 5) open이 비면 길이 없다 (found = false)
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
