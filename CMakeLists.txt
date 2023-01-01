cmake_minimum_required(VERSION 3.24)
project(myCode)

set(CMAKE_CXX_STANDARD 14)


add_executable(myCode
        code/headers/Game.h code/code/Game.cpp code/code/Cell.cpp code/headers/Cell.h code/code/Result.cpp code/headers/Result.h code/code/Out.cpp code/headers/Out.h code/utils.cpp code/utils.h tmpMain.cpp)
