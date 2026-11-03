# Sokoban.exe: 게임 규칙(Sokoban.cpp)과 화면(GridView.cpp). Direct3D를 직접 쓰지 않는다.
add_layer_app(${STEP}_Sokoban ${STEP}_Engine)
copy_to_output(${STEP}_Sokoban ${CMAKE_CURRENT_SOURCE_DIR}/../Render/shaders shaders)
copy_to_output(${STEP}_Sokoban ${CMAKE_CURRENT_SOURCE_DIR}/levels levels)
