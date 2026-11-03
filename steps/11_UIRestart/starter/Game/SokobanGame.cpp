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

void SokobanGame::Restart() {
    // TODO(11): 처음 상태로 되돌린다. 상태가 '진실'이므로 화면은 다음 프레임에 저절로 따라온다.
    //   힌트: initial_을 level_에 복사하고, 이동 횟수와 진행 상태도 처음 값으로.
}

MoveResult SokobanGame::Move(Dir dir) {
    if (state_ == GameState::Cleared) return MoveResult::Blocked;
    const MoveResult result = sokoban::TryMove(level_, dir);
    if (result != MoveResult::Blocked) {
        ++moves_;
        if (sokoban::IsCleared(level_)) state_ = GameState::Cleared;
    }
    return result;
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
