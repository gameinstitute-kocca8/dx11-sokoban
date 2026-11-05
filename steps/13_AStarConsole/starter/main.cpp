// main.cpp - 콘솔 A*
// 사용법:
//   AStarConsole                        maps/maze.txt
//   AStarConsole <맵 파일>              경로를 '*'로 표시
//   AStarConsole <맵 파일> --expanded   탐색했지만 경로가 아닌 칸도 'o'로 표시
#include "AStar.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char** argv) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    const std::string path = argc >= 2 ? argv[1] : "maps/maze.txt";
    const bool showExpanded = argc >= 3 && std::strcmp(argv[2], "--expanded") == 0;

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "맵 파일을 열 수 없음: " << path << '\n';
        return 1;
    }
    std::ostringstream text;
    text << file.rdbuf();

    astar::Grid grid;
    astar::Int2 start;
    astar::Int2 goal;
    std::string error;
    if (!astar::LoadGridFromText(text.str(), grid, start, goal, error)) {
        std::cerr << "맵 오류: " << error << '\n';
        return 1;
    }

    const astar::Result result = astar::FindPath(grid, start, goal);
    std::cout << astar::ToText(grid, start, goal, result, showExpanded);
    if (result.found) {
        std::cout << "경로 길이(이동 횟수): " << result.path.size() - 1;
    } else {
        std::cout << "경로 없음";
    }
    std::cout << "   탐색한 칸: " << result.expanded.size() << " / 빈칸 "
              << std::count(grid.blocked.begin(), grid.blocked.end(), false) << '\n';
    return 0;
}
