# 07_SplitLibraries - Render.lib / Engine.lib / CubeViewer.exe
#   의존 방향: CubeViewer → Engine → Render   (반대 방향 include는 tools/check_layers.py가 막는다)
set(STEP 07)
add_subdirectory(Render)
add_subdirectory(Engine)
add_subdirectory(CubeViewer)
