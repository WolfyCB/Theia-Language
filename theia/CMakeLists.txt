cmake_minimum_required(VERSION 3.25)
project(Theia LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



file(GLOB_RECURSE THEIA_SRC PUBLIC
        "tools.c"
        "tokens.c"
        "lexer.c"
)
add_executable(Theia
	${THEIA_SRC}
        main.cpp
)

