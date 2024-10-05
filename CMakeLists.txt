cmake_minimum_required(VERSION 3.27)
project(FlatOutUCChloeCollection)

SET(CMAKE_SYSTEM_NAME Windows)

SET(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
SET(CMAKE_RC_COMPILER i686-w64-mingw32-windres)
SET(CMAKE_RANLIB i686-w64-mingw32-ranlib)

SET(CMAKE_CXX_STANDARD 20)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -masm=intel -s -static")

add_compile_definitions(IMGUI_USER_CONFIG="../../nya_imconfig.h" NYA_BACKEND_DX9 NYA_NO_FONT)
add_library(FlatOutUCChloeCollection SHARED
        ../nya-common/3rdparty/imgui/imgui.cpp
        ../nya-common/3rdparty/imgui/backends/imgui_impl_dx9.cpp
        ../nya-common/3rdparty/imgui/imgui_draw.cpp
        ../nya-common/3rdparty/imgui/imgui_tables.cpp
        ../nya-common/3rdparty/imgui/imgui_widgets.cpp
        ../nya-common/nya_commonfuncs.cpp
        ../nya-common/nya_commondraw.cpp
        ../nya-common/nya_commonnet.cpp
        ../nya-common/nya_commontimer.cpp
        ../nya-common/nya_dx9_hookbase.cpp
        main.cpp)
target_include_directories(FlatOutUCChloeCollection PUBLIC ../nya-common)
target_include_directories(FlatOutUCChloeCollection PUBLIC ../nya-common/3rdparty)
target_include_directories(FlatOutUCChloeCollection PUBLIC ../nya-common/3rdparty/imgui)
target_include_directories(FlatOutUCChloeCollection PUBLIC ../nya-common/3rdparty/imgui/backends)
target_include_directories(FlatOutUCChloeCollection PUBLIC ../nya-common-fouc)
target_include_directories(FlatOutUCChloeCollection PUBLIC ../FlatOutUCMenuLib/include)
target_link_options(FlatOutUCChloeCollection PRIVATE "-Wl,--exclude-all-symbols")
set_target_properties(FlatOutUCChloeCollection PROPERTIES PREFIX "")
set_target_properties(FlatOutUCChloeCollection PROPERTIES SUFFIX "_gcp.asi")

target_link_libraries(FlatOutUCChloeCollection d3dx9)