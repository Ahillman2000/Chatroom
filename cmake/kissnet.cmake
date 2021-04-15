cmake_minimum_required(VERSION 3.14)
include(FetchContent)
FetchContent_Declare(
        kissnet
        GIT_REPOSITORY https://github.com/HuxyUK/kissnet
        GIT_TAG master)

FetchContent_MakeAvailable(kissnet)