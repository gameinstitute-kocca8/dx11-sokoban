# 학생용 레포 루트. steps/NN_이름/starter/ 를 모두 빌드한다.
# Visual Studio: '폴더 열기'로 이 레포를 열면 CMake 프로젝트로 인식한다. 자세한 방법은 docs/SETUP.md
cmake_minimum_required(VERSION 3.21)
project(DX11Sokoban LANGUAGES CXX)

include(cmake/Course.cmake)
course_setup(${CMAKE_CURRENT_SOURCE_DIR}/common)
course_add_steps("steps/*/starter")
