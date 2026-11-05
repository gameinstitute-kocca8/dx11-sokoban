# 13_AStarConsole - 플랫폼 무관 콘솔 A*
add_library(AStar13 STATIC AStar.cpp AStar.h)
target_include_directories(AStar13 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(AStarConsole main.cpp)
target_link_libraries(AStarConsole PRIVATE AStar13)

add_executable(AStarTests tests.cpp)
target_link_libraries(AStarTests PRIVATE AStar13)
add_test(NAME 13_AStarScenarios COMMAND AStarTests)

add_custom_command(TARGET AStarConsole POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/maps $<TARGET_FILE_DIR:AStarConsole>/maps)
set_target_properties(AStarConsole PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:AStarConsole>)

# AI 검증 과제: ai_review/AStar_ai.cpp ("AI가 짜 준" 버그 있는 A*)로 같은 테스트와 콘솔을 빌드한다.
# 일부러 실패하는 코드이므로 ctest에는 등록하지 않는다.
add_library(AStar13Review STATIC ai_review/AStar_ai.cpp AStar.h)
target_include_directories(AStar13Review PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(AStarReviewTests tests.cpp)
target_link_libraries(AStarReviewTests PRIVATE AStar13Review)

add_executable(AStarReviewConsole main.cpp)
target_link_libraries(AStarReviewConsole PRIVATE AStar13Review)
add_custom_command(TARGET AStarReviewConsole POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/maps $<TARGET_FILE_DIR:AStarReviewConsole>/maps)
set_target_properties(AStarReviewConsole PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:AStarReviewConsole>)
