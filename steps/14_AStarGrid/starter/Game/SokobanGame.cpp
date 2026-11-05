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
    if (history_.empty()) return false;
    level_ = history_.back();
    history_.pop_back();
    --moves_;
    state_ = GameState::Playing;
    return true;
}

MoveResult SokobanGame::Move(Dir dir) {
    if (state_ == GameState::Cleared) return MoveResult::Blocked;
    const sokoban::Level before = level_;
    const MoveResult result = sokoban::TryMove(level_, dir);
    if (result != MoveResult::Blocked) {
        history_.push_back(before);
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
