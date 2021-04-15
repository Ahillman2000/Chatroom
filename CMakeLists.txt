cmake_minimum_required(VERSION 3.17)
project(NetTutorial)
set(CMAKE_CXX_STANDARD 20)
include(cmake/kissnet.cmake)

add_executable(server app/server.cpp)
target_link_libraries(server PRIVATE kissnet $<$<PLATFORM_ID:Linux>:pthread> )
target_compile_options(server PRIVATE
        "-Wall" "-Wextra" "-Wshadow" "-Wnon-virtual-dtor"
        "-Wold-style-cast" "-Wunused" "-Wno-documentation"
        "-Woverloaded-virtual" "-Wpedantic" "-Wconversion"
        "-Wdouble-promotion" "-Wundef" "-Wnull-dereference"
        "-Wno-c++98-compat" "-Wredundant-move"
        "-Wpessimizing-move" "-Wno-c++98-compat-pedantic"
        "-Wno-documentation-unknown-command" "-Wno-double-promotion"
        "-Werror" "-ferror-limit=100" $<$<PLATFORM_ID:Windows>:/EHsc> )

add_executable(client app/client.cpp)
target_link_libraries(client PRIVATE kissnet $<$<PLATFORM_ID:Linux>:pthread> )
target_compile_options(client PRIVATE
        "-Wall" "-Wextra" "-Wshadow" "-Wnon-virtual-dtor"
        "-Wold-style-cast" "-Wunused" "-Wno-documentation"
        "-Woverloaded-virtual" "-Wpedantic" "-Wconversion"
        "-Wdouble-promotion" "-Wundef" "-Wnull-dereference"
        "-Wno-c++98-compat" "-Wredundant-move"
        "-Wpessimizing-move" "-Wno-c++98-compat-pedantic"
        "-Wno-documentation-unknown-command" "-Wno-double-promotion"
        "-Werror" "-ferror-limit=100" $<$<PLATFORM_ID:Windows>:/EHsc>)