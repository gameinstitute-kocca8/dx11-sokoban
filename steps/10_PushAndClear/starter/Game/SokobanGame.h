// SokobanGame.h - 한 판의 흐름: 레벨 + 이동 횟수 + 진행 상태
//
// 00단계의 TryMove는 '한 번의 이동' 규칙이다. 이 클래스는 그 위에 '한 판'의 규칙을 얹는다.
//   - 성공한 이동(Moved, Pushed)만 센다
//   - 모든 목표가 채워지면 Cleared가 되고, 그 뒤로는 이동을 받지 않는다
// 그리기와 입력은 모른다. 그래서 00단계처럼 콘솔 테스트로도 검증할 수 있다.
#pragma once

#include <string>

#include "Game/Sokoban.h"

namespace game {

enum class GameState { Playing, Cleared };

class SokobanGame {
public:
    bool Load(const std::wstring& relativePath);  // 레벨 파일을 읽고 처음 상태로

    // 한 번 이동을 시도한다. Cleared 상태면 아무것도 하지 않고 Blocked.
    sokoban::MoveResult Move(sokoban::Dir dir);

    const sokoban::Level& GetLevel() const { return level_; }
    int Moves() const { return moves_; }
    GameState State() const { return state_; }

private:
    sokoban::Level initial_;  // 처음 상태 (11단계 Restart에서 쓴다)
    sokoban::Level level_;    // 지금 상태
    int moves_ = 0;
    GameState state_ = GameState::Playing;
};

// 검증용 이동 문자열의 한 글자를 지도 방향으로. w 위, a 왼쪽, s 아래, d 오른쪽. 그 외는 false
bool ScriptCharToDir(char c, sokoban::Dir& out);

}  // namespace game
