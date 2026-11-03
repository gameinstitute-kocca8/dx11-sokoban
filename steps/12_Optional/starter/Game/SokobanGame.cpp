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
    history_.clear();
    state_ = sokoban::IsCleared(level_) ? GameState::Cleared : GameState::Playing;
    return true;
}

void SokobanGame::Restart() {
    level_ = initial_;
    moves_ = 0;
    state_ = GameState::Playing;
    history_.clear();
}

bool SokobanGame::Undo() {
    // TODO(12): 기록의 마지막 상태로 돌아간다. 이동 횟수를 하나 줄이고, Clear였다면 다시 Playing.
    //   힌트: history_가 비어 있으면 false. back()을 level_에 넣고 pop_back()
    return false;
}

MoveResult SokobanGame::Move(Dir dir) {
    if (state_ == GameState::Cleared) return MoveResult::Blocked;
    const sokoban::Level before = level_;
    const MoveResult result = sokoban::TryMove(level_, dir);
    if (result != MoveResult::Blocked) {
        // TODO(12): 성공한 이동이면 '이동 직전' 상태를 기록에 넣는다. 막힌 이동은 기록하지 않는다 (왜?)
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
