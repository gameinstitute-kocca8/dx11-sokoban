// Sokoban.cpp
#include "Game/Sokoban.h"

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
    const Int2 offset = DirToOffset(dir);
    const Int2 next = level.player + offset;

    if (level.TerrainAt(next) == Terrain::Wall) return MoveResult::Blocked;

    if (level.OccupantAt(next) == Occupant::Empty) {
        level.SetOccupant(level.player, Occupant::Empty);
        level.SetOccupant(next, Occupant::Player);
        level.player = next;
        return MoveResult::Moved;
    }

    if (level.OccupantAt(next) == Occupant::Crate) {
        const Int2 beyond = next + offset;
        if (level.TerrainAt(beyond) == Terrain::Wall) return MoveResult::Blocked;
        if (level.OccupantAt(beyond) != Occupant::Empty) return MoveResult::Blocked;

        level.SetOccupant(beyond, Occupant::Crate);
        level.SetOccupant(level.player, Occupant::Empty);
        level.SetOccupant(next, Occupant::Player);
        level.player = next;
        return MoveResult::Pushed;
    }
    return MoveResult::Blocked;
}

bool IsCleared(const Level& level) {
    int goalCount = 0;
    for (std::size_t i = 0; i < level.terrain.size(); ++i) {
        if (level.terrain[i] != Terrain::Goal) continue;
        ++goalCount;
        if (level.occupant[i] != Occupant::Crate) return false;
    }
    return goalCount > 0;
}

}  // namespace sokoban
