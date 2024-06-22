set(SOURCES audio.cpp document.cpp file.cpp filecontent.cpp filesystem.cpp image.cpp video.cpp)

set(LIBRARY_NAME filelib)
set(EXECUTABLE_NAME filesys)


add_library(${LIBRARY_NAME} ${SOURCES})
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_20)

add_executable(${EXECUTABLE_NAME} test.cpp)
target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY_NAME})

