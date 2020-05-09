cmake_minimum_required(VERSION 3.10)

project(ray_tracing_tutorial VERSION 0.1)

set(EXECUTABLE_NAME "ray_tracing_tutorial")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Source files with Configure_depends, triggers a rebuild if new files are found
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${SRC_DIR}/*.h")
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE EDITOR_CFG_FILES CONFIGURE_DEPENDS "${SRC_DIR}/.editorconfig")

# MSVC specifics
if(MSVC)
    # For filters in VS 2019
    # All sources that are listed here must be added to a target to actually appear in Visual Studio
    source_group(
        TREE "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES ${HEADER_FILES} ${SRC_FILES} ${EDITOR_CFG_FILES}
    )
endif()

# Executable
add_executable(${EXECUTABLE_NAME} ${HEADER_FILES} ${SRC_FILES} ${EDITOR_CFG_FILES})

# MSVC specifics
if(MSVC)
    # Remove the console when running the program
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

    # Max warning level, warning = errors
    # C4710 is linked to function not being inlined, why is this a warning ?
    # C4711 is linked to function being inlined, why is this a warning ?
    # C4820 is linked to padding added to struct which we don't care about here
    # C5045 is linked to spectre mitigation, we don't care about it here
    target_compile_options(${EXECUTABLE_NAME} PRIVATE /Wall /WX /wd4710 /wd4711 /wd4820 /wd5045)

    # MSVC has a problem with some min and max macros colliding with the functions from algorithm : http://www.suodenjoki.dk/us/archive/2010/min-max.htm
    add_compile_definitions(NOMINMAX)

    # Set working directory to easily access shader bytecode
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

    foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        STRING (REGEX REPLACE "/RTC[^ ]*" "" ${flag_var} "${${flag_var}}")
    endforeach(flag_var)
endif()
