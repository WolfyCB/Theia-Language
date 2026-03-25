cmake_minimum_required(VERSION 3.25)
project(Sphen LANGUAGES C)

set(CMAKE_C_STANDARD 17)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE SPHEN_SRC PUBLIC
        "str.c"
        "debug.c"
        "tokens.c"
        "lexer.c"
        "ast.c"
        "parser.c"
)
add_executable(Sphen
	${SPHEN_SRC}
	main.c
)

