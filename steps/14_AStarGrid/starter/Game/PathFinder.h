// PathFinder.h - 소코반 상태와 13단계 A*를 잇는 다리
//
// A*(Game/AStar.*)는 "막힌 칸"만 안다. 소코반에서 무엇이 막힌 칸인지는 여기서 정한다.
// 자동 이동은 상자를 밀지 않는다. 그래서 벽과 상자가 모두 막힌 칸이다.
#pragma once

#include <vector>

#include "Game/AStar.h"
#include "Game/Sokoban.h"

namespace game {

// 지금 상태에서 플레이어가 '밀지 않고' 걸어갈 수 있는지를 격자로
astar::Grid MakeWalkGrid(const sokoban::Level& level);

// 경로(칸 목록)를 이동 방향 목록으로. 이웃하지 않은 두 칸이 나오면 false
bool PathToDirs(const std::vector<astar::Int2>& path, std::vector<sokoban::Dir>& dirs);

// 좌표 변환 (둘 다 x, y 정수 쌍이지만 서로 다른 모듈의 타입이다)
inline astar::Int2 ToAStar(sokoban::Int2 p) { return astar::Int2{p.x, p.y}; }

}  // namespace game
