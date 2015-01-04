cmake_minimum_required (VERSION 2.8)
project (midi-synth)
set (VERSION_MAJOR 0)
set (VERSION_MINOR 0)
set (TITLE "${PROJECT_NAME}")
set (CMAKE_CXX_FLAGS "-std=c++11 -O2 -lSDL2")

find_package(ALSA REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}/include/")

add_executable(
	${PROJECT_NAME}
	src/main.cpp src/keyboard.cpp src/display.cpp src/audio.cpp
)

target_link_libraries(${PROJECT_NAME} ${ALSA_LIBRARIES})
