set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB IMGUI_SRC
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_draw.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_tables.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_widgets.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_demo.cpp"  
)

file(GLOB IMGUI_BACKENDS
    "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_glfw.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_vulkan.cpp"
)

find_package(Vulkan REQUIRED)
find_package(Freetype REQUIRED)
find_package(assimp REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(
    ${Vulkan_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    "${CMAKE_SOURCE_DIR}/External"
    "${CMAKE_SOURCE_DIR}/EngineCore/Include"
    "${CMAKE_SOURCE_DIR}/EngineCore/Include/ECS"
    "${CMAKE_SOURCE_DIR}/EngineCore/Include/Voxels"
    "${CMAKE_SOURCE_DIR}/EngineCore/Include/Vulkan"
    "${CMAKE_SOURCE_DIR}/EngineCore/Include/Utils"
    "${CMAKE_SOURCE_DIR}/External/imgui"
    "${CMAKE_SOURCE_DIR}/External/imgui/backends"
)

link_directories(
    ${FREETYPE_LIBRARY_DIRS}
    ${ASSIMP_LIBRARY_DIRS}
    ${Vulkan_LIBRARY_DIRS}
    ${GLFW_LIBRARY_DIRS}
)

file(GLOB ENGINE_SRC "${CMAKE_SOURCE_DIR}/EngineCore/src/*.cpp" "${CMAKE_SOURCE_DIR}/EngineCore/src/*/*.cpp")

add_executable(GameEngine
    ${ENGINE_SRC}
    ${IMGUI_SRC}
    ${IMGUI_BACKENDS}
)

#target_compile_definitions(GameEngine PRIVATE NDEBUG)

target_link_libraries(GameEngine
    ${Vulkan_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${ASSIMP_LIBRARIES}
    glfw
    Vulkan::Vulkan
)

add_custom_command(TARGET GameEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Assets $<TARGET_FILE_DIR:GameEngine>/Assets
)

if (WIN32)
add_custom_command(TARGET GameEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Running shader compiler"
    COMMAND cmd /c "${CMAKE_SOURCE_DIR}/shaders/compile.bat" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/shaders
)
elseif(UNIX)
find_program(glslc NAMES glslc)
if(NOT glslc)
    message(FATAL_ERROR "glslc not found! Please install Vulkan SDK and ensure glslc is in your PATH.")
else()
add_custom_command(TARGET GameEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Running shader compiler"
    COMMAND /bin/sh "${CMAKE_SOURCE_DIR}/shaders/compile.sh" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/shaders
)
endif()
endif()

add_custom_command(TARGET GameEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:GameEngine>/shaders
)