# 00_TextSokoban - 플랫폼 무관 콘솔 소코반
add_library(SokobanRules00 STATIC Sokoban.cpp Sokoban.h)
target_include_directories(SokobanRules00 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(TextSokoban main.cpp)
target_link_libraries(TextSokoban PRIVATE SokobanRules00)

add_executable(TextSokobanTests tests.cpp)
target_link_libraries(TextSokobanTests PRIVATE SokobanRules00)
add_test(NAME 00_RuleScenarios COMMAND TextSokobanTests)

# 레벨 파일을 실행 파일 옆으로 복사
add_custom_command(TARGET TextSokoban POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/levels $<TARGET_FILE_DIR:TextSokoban>/levels)
set_target_properties(TextSokoban PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:TextSokoban>)
