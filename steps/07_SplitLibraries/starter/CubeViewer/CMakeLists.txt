# CubeViewer.exe: Engine만 링크한다 (Render는 Engine을 통해 따라온다)
add_layer_app(${STEP}_CubeViewer ${STEP}_Engine)
copy_to_output(${STEP}_CubeViewer ${CMAKE_CURRENT_SOURCE_DIR}/../Render/shaders shaders)
