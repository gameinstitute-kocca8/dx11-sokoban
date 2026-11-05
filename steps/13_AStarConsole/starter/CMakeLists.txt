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
