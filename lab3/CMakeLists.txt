cmake_minimum_required(VERSION 3.5)

project(Lab03)
set(TARGET ${CMAKE_PROJECT_NAME})
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)

set(SOURCES
	"src/main.cpp"
	"src/utils.cpp"
	"depends/imgui_impl_glfw.cpp"
	"depends/imgui_impl_opengl3.cpp"
	"depends/imgui.cpp"
	"depends/imgui_demo.cpp"
	"depends/imgui_draw.cpp"
	"depends/imgui_widgets.cpp"
	)

add_executable(${TARGET} ${SOURCES})

target_include_directories(${TARGET} PRIVATE
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/depends
	${GLFW_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	${GLM_INCLUDE_DIRS/../include}
	)
target_link_libraries(${TARGET} ${OPENGL_LIBRARIES} glfw GLEW::GLEW)

