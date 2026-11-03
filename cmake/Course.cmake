# Course.cmake - 강사용/학생용 레포가 함께 쓰는 빌드 설정
#
#   course_setup(<common 폴더>)        컴파일 옵션, DirectXTK, StepCommon 라이브러리 준비
#   course_add_steps(<glob 패턴>)      패턴에 맞는 단계 폴더를 모두 add_subdirectory
#   add_dx11_step(<이름>)              단계 폴더의 CMakeLists.txt에서 호출. 폴더의 *.cpp/*.h + shaders/ 로 실행 파일 생성
#
#   07단계부터 (레이어 분리)
#   add_layer_library(<타깃> <의존 타깃...>)   현재 폴더의 *.cpp/*.h로 정적 라이브러리. include 기준은 단계 폴더
#                                              → 다른 레이어에서 #include "Render/Renderer.h" 처럼 쓴다
#   add_layer_app(<타깃> <의존 타깃...>)       현재 폴더의 *.cpp/*.h로 실행 파일
#   copy_to_output(<타깃> <원본 폴더> <대상 이름>)  빌드 후 원본 폴더를 실행 파일 옆으로 복사 (셰이더, 레벨, 폰트)
include_guard(GLOBAL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    # /utf-8: 소스의 한글 주석과 문자열을 UTF-8로 읽는다. 빠지면 한글 로그가 깨진다.
    add_compile_options(/utf-8 /W3 /permissive-)
endif()

macro(course_setup common_dir)
    enable_testing()
    if(WIN32)
        include(FetchContent)
        # DirectXTK: SimpleMath, ScreenGrab, (이후 단계) Keyboard, Mouse, WICTextureLoader, SpriteFont, PrimitiveBatch
        set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
        set(BUILD_XAUDIO_WIN8 OFF CACHE BOOL "" FORCE)
        set(BUILD_XAUDIO_WIN10 OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(DirectXTK
            GIT_REPOSITORY https://github.com/microsoft/DirectXTK.git
            GIT_TAG oct2025
            GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(DirectXTK)

        add_library(StepCommon STATIC ${common_dir}/StepApp.cpp ${common_dir}/StepApp.h)
        target_include_directories(StepCommon PUBLIC ${common_dir})
        target_compile_definitions(StepCommon PUBLIC UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
        target_link_libraries(StepCommon PUBLIC DirectXTK d3d11 dxgi d3dcompiler)
    endif()
endmacro()

function(add_dx11_step name)
    set(target ${name})
    # 단계 폴더의 모든 .cpp/.h (main.cpp, 05단계부터 Camera.cpp 등)
    file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
    add_executable(${target} WIN32 ${sources})
    target_link_libraries(${target} PRIVATE StepCommon)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
        file(GLOB shaders CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.hlsl)
        target_sources(${target} PRIVATE ${shaders})
        # 셰이더는 실행 중에 D3DCompileFromFile로 컴파일한다. Visual Studio가 빌드 때 따로 컴파일하지 않게 한다.
        set_source_files_properties(${shaders} PROPERTIES VS_TOOL_OVERRIDE "None")
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${target}>/shaders)
    endif()
    set_target_properties(${target} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${target}>)
endfunction()

# pattern 예: "solutions/*" 또는 "steps/*/starter"
# Windows가 아니면 콘솔 단계(00_)만 추가한다.
function(course_add_steps pattern)
    file(GLOB step_dirs LIST_DIRECTORIES true ${CMAKE_CURRENT_SOURCE_DIR}/${pattern})
    list(SORT step_dirs)
    foreach(dir IN LISTS step_dirs)
        if(NOT EXISTS ${dir}/CMakeLists.txt)
            continue()
        endif()
        file(RELATIVE_PATH rel ${CMAKE_CURRENT_SOURCE_DIR} ${dir})
        string(REGEX MATCH "(^|/)([0-9][0-9])_" step_match ${rel})
        set(step_number ${CMAKE_MATCH_2})
        if(NOT WIN32 AND NOT step_number STREQUAL "00")
            continue()
        endif()
        message(STATUS "단계 추가: ${rel}")
        add_subdirectory(${dir} ${CMAKE_CURRENT_BINARY_DIR}/${rel})
    endforeach()
endfunction()

# ---------------------------------------------------------------------------
# 07단계부터: Render.lib / Engine.lib / 실행 파일
# ---------------------------------------------------------------------------
function(add_layer_library target)
    file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
    add_library(${target} STATIC ${sources})
    # 단계 폴더를 include 기준으로 삼아 "Render/Renderer.h", "Engine/Camera.h"처럼 레이어 이름이 드러나게 한다
    get_filename_component(step_root ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
    target_include_directories(${target} PUBLIC ${step_root})
    target_link_libraries(${target} PUBLIC ${ARGN})
endfunction()

function(add_layer_app target)
    file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
    add_executable(${target} WIN32 ${sources})
    get_filename_component(step_root ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
    target_include_directories(${target} PRIVATE ${step_root})
    target_link_libraries(${target} PRIVATE ${ARGN})
    set_target_properties(${target} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${target}>)
endfunction()

function(copy_to_output target source_dir dest_name)
    file(GLOB files CONFIGURE_DEPENDS ${source_dir}/*)
    set_source_files_properties(${files} PROPERTIES VS_TOOL_OVERRIDE "None" HEADER_FILE_ONLY TRUE)
    target_sources(${target} PRIVATE ${files})
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${source_dir} $<TARGET_FILE_DIR:${target}>/${dest_name})
endfunction()
