find_program(
    CLANG_TIDY_EXE
    NAMES "clang-tidy"
    DOC "Path to clang-tidy executable"
)

if(NOT CLANG_TIDY_EXE)
    message(STATUS "clang-tidy not found.")
else()
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}"
        "-checks=*,-clang-analyzer-alpha.*,-misc-unused-parameters,-google-build-using-namespace,-llvm-*,-google-*"
        "-p=."
        "-extra-arg-before=-std=c++17"
        "-header-filter=\\.hpp$"
    )
    set(CMAKE_CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
endif()

macro(enable_clang_tidy_for)
    if(CLANG_TIDY_EXE)
        foreach(arg IN LISTS ARGN)
            set_target_properties(arg PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
        endforeach()
    endif()
endmacro()
