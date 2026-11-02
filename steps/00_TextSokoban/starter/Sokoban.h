// Sokoban.h
// 소코반 규칙의 전부. 렌더링, 입력 장치, 파일 시스템을 전혀 모른다.
// 이 파일은 00단계에서 만들고, 07단계 이후 3D 게임에서 그대로 재사용한다.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sokoban {

// 바닥 층: 움직이지 않는 지형
enum class Terrain : std::uint8_t { Floor, Wall, Goal };

// 위 층: 칸 위에 올라선 것
enum class Occupant : std::uint8_t { Empty, Player, Crate };

enum class Dir { Up, Down, Left, Right };

enum class MoveResult {
    Moved,   // 빈 칸으로 이동
    Pushed,  // 상자를 한 칸 밀고 이동
    Blocked  // 이동 불가 (상태 변화 없음)
};

struct Int2 {
    int x = 0;
    int y = 0;
};

inline bool operator==(Int2 a, Int2 b) { return a.x == b.x && a.y == b.y; }
inline Int2 operator+(Int2 a, Int2 b) { return {a.x + b.x, a.y + b.y}; }

// 좌표계: x는 오른쪽(+), y는 아래쪽(+). 텍스트 맵의 첫 줄이 y = 0.
// 3D로 옮길 때의 대응은 07단계 TASK.md에서 정한다.
Int2 DirToOffset(Dir dir);

// 게임 상태 전체. 화면에 보이는 모든 것은 이 값에서 계산된다.
struct Level {
    int width = 0;
    int height = 0;
    std::vector<Terrain> terrain;    // width * height
    std::vector<Occupant> occupant;  // width * height
    Int2 player;

    bool InBounds(Int2 p) const { return p.x >= 0 && p.y >= 0 && p.x < width && p.y < height; }
    int Index(Int2 p) const { return p.y * width + p.x; }

    // 맵 밖은 벽으로 취급한다.
    Terrain TerrainAt(Int2 p) const { return InBounds(p) ? terrain[Index(p)] : Terrain::Wall; }
    Occupant OccupantAt(Int2 p) const { return InBounds(p) ? occupant[Index(p)] : Occupant::Empty; }
    void SetOccupant(Int2 p, Occupant o) { occupant[Index(p)] = o; }
};

// 레벨 텍스트 형식은 docs/LEVEL-FORMAT.md 참고. 실패하면 false와 error 메시지.
bool LoadLevelFromText(const std::string& text, Level& out, std::string& error);

// Level을 같은 형식의 텍스트로 되돌린다. 테스트와 콘솔 출력에 쓴다.
std::string ToText(const Level& level);

// 플레이어를 dir 방향으로 한 칸 움직인다. 규칙:
//  1) 앞 칸이 벽이면 Blocked
//  2) 앞 칸이 비어 있으면 이동하고 Moved
//  3) 앞 칸에 상자가 있고, 그 뒤 칸이 벽도 상자도 아니면 상자를 밀고 Pushed
//  4) 그 외에는 Blocked
// Blocked이면 level은 전혀 바뀌지 않아야 한다.
MoveResult TryMove(Level& level, Dir dir);

// 모든 Goal 칸 위에 상자가 있으면 true. Goal이 하나도 없는 레벨은 false.
bool IsCleared(const Level& level);

}  // namespace sokoban
