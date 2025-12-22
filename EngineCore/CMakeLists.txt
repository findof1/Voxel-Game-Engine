set(CMAKE_CXX_STANDARD 17)
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

include_directories(
    "C:/msys64/mingw64/include"
    "C:/msys64/mingw64/include/freetype2"
    "C:/VulkanSDK/1.4.321.1/Include"
    "C:/msys64/mingw64/include/glm"
    "${CMAKE_SOURCE_DIR}/External"
    "${CMAKE_SOURCE_DIR}/EngineCore/Include"
    "${CMAKE_SOURCE_DIR}/EngineCore/Include/ECS"
    "${CMAKE_SOURCE_DIR}/EngineCore/Include/Vulkan"
    "${CMAKE_SOURCE_DIR}/EngineCore/Include/Utils"
    "${CMAKE_SOURCE_DIR}/External/imgui"
    "${CMAKE_SOURCE_DIR}/External/imgui/backends"
)

link_directories(C:/msys64/mingw64/lib C:/VulkanSDK/1.4.321.1/Lib)

file(GLOB ENGINE_SRC "${CMAKE_SOURCE_DIR}/EngineCore/src/*.cpp" "${CMAKE_SOURCE_DIR}/EngineCore/src/*/*.cpp")

add_executable(GameEngine
    ${ENGINE_SRC}
    ${IMGUI_SRC}
    ${IMGUI_BACKENDS}
)

#target_compile_definitions(GameEngine PRIVATE NDEBUG)

target_link_libraries(GameEngine
    glfw3
    vulkan-1
    freetype
    LinearMath
    ws2_32
    assimp
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
add_custom_command(TARGET GameEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Running shader compiler"
    COMMAND /bin/sh "${CMAKE_SOURCE_DIR}/shaders/compile.sh" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/shaders
)
endif()

add_custom_command(TARGET GameEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:GameEngine>/shaders
)