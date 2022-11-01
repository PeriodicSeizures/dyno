cmake_minimum_required(VERSION 3.21)
project(dyno)

set(CMAKE_CXX_STANDARD 20)

############
# Libraries
############
find_package(Threads REQUIRED)
find_package(asio CONFIG REQUIRED)

# use this when building as a library
#add_library(${PROJECT_NAME} server.cpp user_session.cpp)

# using this to test
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME} src/server.cpp src/user_session.cpp Main.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC Threads::Threads
    asio asio::asio
)
