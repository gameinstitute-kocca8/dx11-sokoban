// AStar.h - 격자 위의 A* 길찾기 (4방향, 맨해튼 거리)
//
// 이 파일은 소코반도, 렌더링도 모른다. "막힌 칸이 어디인가"만 알면 된다.
// 13단계에서 콘솔로 완성한 것을 그대로 가져왔다. 내 13단계 코드로 바꿔 넣어도 된다.
#pragma once

#include <string>
#include <vector>

namespace astar {

struct Int2 {
    int x = 0;
    int y = 0;
};

inline bool operator==(Int2 a, Int2 b) { return a.x == b.x && a.y == b.y; }
inline bool operator!=(Int2 a, Int2 b) { return !(a == b); }

// 격자: 칸마다 막혔는지(true)만 기억한다. 좌표계는 00단계 소코반과 같다 (x 오른쪽, y 아래쪽).
struct Grid {
    int width = 0;
    int height = 0;
    std::vector<bool> blocked;  // width * height

    bool InBounds(Int2 p) const { return p.x >= 0 && p.y >= 0 && p.x < width && p.y < height; }
    int Index(Int2 p) const { return p.y * width + p.x; }
    bool Passable(Int2 p) const { return InBounds(p) && !blocked[Index(p)]; }
};

struct Result {
    bool found = false;
    std::vector<Int2> path;      // start부터 goal까지 (둘 다 포함). 못 찾으면 비어 있다
    std::vector<Int2> expanded;  // 닫힌 목록(closed)에 들어간 순서. 탐색이 얼마나 넓게 퍼졌는지 보여 준다
};

// 휴리스틱: 두 칸 사이의 맨해튼 거리 |dx| + |dy|
int Heuristic(Int2 a, Int2 b);

// start에서 goal까지 4방향, 한 칸 비용 1인 최단 경로를 찾는다.
//   start나 goal이 막혀 있거나 맵 밖이면 found = false.
//   start == goal이면 path = { start }.
Result FindPath(const Grid& grid, Int2 start, Int2 goal);

// 맵 텍스트 읽기. # 벽, . 또는 공백 빈칸, S 시작, G 목표. 실패하면 false와 error.
bool LoadGridFromText(const std::string& text, Grid& grid, Int2& start, Int2& goal, std::string& error);

// 맵과 경로를 텍스트로. 경로 칸은 '*', 시작 S, 목표 G. showExpanded면 탐색했지만 경로가 아닌 칸을 'o'로.
std::string ToText(const Grid& grid, Int2 start, Int2 goal, const Result& result, bool showExpanded);

}  // namespace astar
