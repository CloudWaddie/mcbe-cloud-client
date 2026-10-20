cmake_minimum_required(VERSION 4.3)
project(cloud-client)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
include(cmake/CPM.cmake)

# libraries we need
CPMAddPackage("gh:BasedInc/libhat@0.11.1")
CPMAddPackage("gh:TsudaKageyu/minhook@1.3.4")
CPMAddPackage(
        URI "gh:BasedInc/kiero#269860311ac04b0ee89e68f68bca55abd02a3390" # Dec 2, 2025
        OPTIONS
        "KIERO_INCLUDE_D3D11 ON"
        "KIERO_INCLUDE_D3D12 ON"
        "KIERO_BUILD_MINHOOK ON"
        "KIERO_MINHOOK_EXTERNAL ON"
        "KIERO_BUILD_EXAMPLES OFF"
)

CPMAddPackage("gh:ocornut/imgui@1.92.9")
CPMAddPackage("gh:DisabledMallis/NuvolaEventSystem@1.2.1")

add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

# add everything
File(GLOB_RECURSE dllsrc CONFIGURE_DEPENDS
        "src/*.h"
        "src/*.cpp"
)

add_library(cloud-client SHARED ${dllsrc})
target_include_directories(cloud-client PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(cloud-client PRIVATE
        kiero::kiero-minhook libhat::libhat minhook imgui d3d11 dxgi d3d12 d3dcompiler RuntimeObject.lib GameInput nuvola_event_system)