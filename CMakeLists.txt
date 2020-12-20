cmake_minimum_required(VERSION 3.17)
project(Kursovaya)

set(CMAKE_CXX_STANDARD 14)

add_executable(Kursovaya main.cpp  "menu.h" "game.h" "game.cpp" "menu.cpp")