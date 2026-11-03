// SokobanGame.cpp
#include "Game/SokobanGame.h"

#include "Game/GridView.h"  // LoadLevelFile

using sokoban::Dir;
using sokoban::MoveResult;

namespace game {

bool SokobanGame::Load(const std::wstring& relativePath) {
    sokoban::Level loaded;
    if (!LoadLevelFile(relativePath, loaded)) return false;
    initial_ = loaded;
    level_ = loaded;
    moves_ = 0;
    state_ = sokoban::IsCleared(level_) ? GameState::Cleared : GameState::Playing;
    return true;
}

MoveResult SokobanGame::Move(Dir dir) {
    // TODO(10): 한 판의 규칙을 적용한다.
    //   힌트: 1) Cleared면 Blocked를 돌려주고 끝  2) TryMove  3) Blocked가 아니면 moves_ 증가
    //   힌트: 4) 이동 뒤 IsCleared면 state_ = Cleared. 판정은 이동이 성공했을 때만 하면 된다 (왜?)
    (void)dir;
    return MoveResult::Blocked;
}

bool ScriptCharToDir(char c, Dir& out) {
    switch (c) {
        case 'w': out = Dir::Up; return true;
        case 'a': out = Dir::Left; return true;
        case 's': out = Dir::Down; return true;
        case 'd': out = Dir::Right; return true;
        default: return false;
    }
}

}  // namespace game
