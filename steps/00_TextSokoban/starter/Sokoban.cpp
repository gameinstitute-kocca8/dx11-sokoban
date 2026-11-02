// Sokoban.cpp
#include "Sokoban.h"

#include <sstream>

namespace sokoban {

Int2 DirToOffset(Dir dir) {
    switch (dir) {
        case Dir::Up: return {0, -1};
        case Dir::Down: return {0, 1};
        case Dir::Left: return {-1, 0};
        case Dir::Right: return {1, 0};
    }
    return {0, 0};
}

// ---------------------------------------------------------------------------
// 레벨 읽기/쓰기 (제공 코드: 형식은 docs/LEVEL-FORMAT.md에서 고정)
// ---------------------------------------------------------------------------

bool LoadLevelFromText(const std::string& text, Level& out, std::string& error) {
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty() && line[0] == ';') continue;  // 주석 줄
        lines.push_back(line);
    }
    // 앞뒤 빈 줄 제거
    while (!lines.empty() && lines.back().find_first_not_of(' ') == std::string::npos) lines.pop_back();
    while (!lines.empty() && lines.front().find_first_not_of(' ') == std::string::npos) lines.erase(lines.begin());
    if (lines.empty()) {
        error = "빈 레벨";
        return false;
    }

    Level level;
    level.height = static_cast<int>(lines.size());
    for (const std::string& l : lines) {
        if (static_cast<int>(l.size()) > level.width) level.width = static_cast<int>(l.size());
    }
    level.terrain.assign(level.width * level.height, Terrain::Floor);
    level.occupant.assign(level.width * level.height, Occupant::Empty);

    int playerCount = 0;
    for (int y = 0; y < level.height; ++y) {
        for (int x = 0; x < level.width; ++x) {
            const char c = x < static_cast<int>(lines[y].size()) ? lines[y][x] : ' ';
            const Int2 p{x, y};
            const int i = level.Index(p);
            switch (c) {
                case '#': level.terrain[i] = Terrain::Wall; break;
                case ' ':
                case '-': level.terrain[i] = Terrain::Floor; break;
                case '.': level.terrain[i] = Terrain::Goal; break;
                case '$': level.occupant[i] = Occupant::Crate; break;
                case '*':
                    level.terrain[i] = Terrain::Goal;
                    level.occupant[i] = Occupant::Crate;
                    break;
                case '@':
                    level.occupant[i] = Occupant::Player;
                    level.player = p;
                    ++playerCount;
                    break;
                case '+':
                    level.terrain[i] = Terrain::Goal;
                    level.occupant[i] = Occupant::Player;
                    level.player = p;
                    ++playerCount;
                    break;
                default:
                    error = std::string("알 수 없는 문자 '") + c + "' (" + std::to_string(x) + ", " +
                            std::to_string(y) + ")";
                    return false;
            }
        }
    }
    if (playerCount != 1) {
        error = "플레이어(@ 또는 +)는 정확히 1개여야 함. 현재 " + std::to_string(playerCount) + "개";
        return false;
    }
    out = level;
    return true;
}

std::string ToText(const Level& level) {
    std::string result;
    for (int y = 0; y < level.height; ++y) {
        std::string row;
        for (int x = 0; x < level.width; ++x) {
            const Int2 p{x, y};
            const Terrain t = level.TerrainAt(p);
            const Occupant o = level.OccupantAt(p);
            char c = ' ';
            if (t == Terrain::Wall) c = '#';
            else if (o == Occupant::Player) c = (t == Terrain::Goal) ? '+' : '@';
            else if (o == Occupant::Crate) c = (t == Terrain::Goal) ? '*' : '$';
            else if (t == Terrain::Goal) c = '.';
            row += c;
        }
        while (!row.empty() && row.back() == ' ') row.pop_back();
        result += row;
        result += '\n';
    }
    return result;
}

// ---------------------------------------------------------------------------
// 규칙 (00단계 과제)
// ---------------------------------------------------------------------------

MoveResult TryMove(Level& level, Dir dir) {
    // TODO(00): Sokoban.h의 규칙 1~4를 구현한다.
    //   힌트: 먼저 next = player + offset, beyond = next + offset 두 좌표를 구한다.
    //   힌트: 상태를 바꾸기 전에 '움직일 수 있는가'를 모두 판정한다. Blocked면 아무것도 바꾸지 않는다.
    //   힌트: 상자를 밀 때는 상자 → 플레이어 순서로 옮기고, level.player도 갱신한다.
    (void)level;
    (void)dir;
    return MoveResult::Blocked;
}

bool IsCleared(const Level& level) {
    // TODO(00): 모든 Goal 칸에 상자가 있는지 검사한다. Goal이 0개면 false.
    //   힌트: terrain과 occupant는 같은 인덱스를 쓴다. 두 배열을 함께 돌면 된다.
    (void)level;
    return false;
}

}  // namespace sokoban
