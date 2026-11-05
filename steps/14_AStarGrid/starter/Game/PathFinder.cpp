// PathFinder.cpp
#include "Game/PathFinder.h"

using sokoban::Dir;
using sokoban::Level;
using sokoban::Occupant;
using sokoban::Terrain;

namespace game {

astar::Grid MakeWalkGrid(const Level& level) {
    astar::Grid grid;
    grid.width = level.width;
    grid.height = level.height;
    grid.blocked.assign(level.width * level.height, false);
    // TODO(14): 벽과 상자가 있는 칸을 막힌 칸으로 표시한다. 플레이어가 서 있는 칸은 막지 않는다 (시작 칸이므로).
    //   힌트: terrain과 occupant는 같은 인덱스를 쓴다 (00단계 IsCleared와 같은 방식)
    return grid;
}

bool PathToDirs(const std::vector<astar::Int2>& path, std::vector<Dir>& dirs) {
    dirs.clear();
    // TODO(14): 이웃한 두 칸의 차이(dx, dy)를 방향으로 바꾼다. (1,0) 오른쪽, (-1,0) 왼쪽, (0,1) 아래, (0,-1) 위
    //   힌트: 경로가 칸 N개면 방향은 N-1개다. 그 밖의 차이가 나오면 경로가 잘못된 것이므로 false
    return true;
}

}  // namespace game
