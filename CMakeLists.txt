project(MemoryGtk3 C)
cmake_minimum_required(VERSION 3.6)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# set warnings
if(MSVC)
    set(CMAKE_C_FLAGS "/W2")
else()
    set(CMAKE_C_FLAGS "-Wall -Wextra -Wno-unused-parameter")
endif()

set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3")

include(CheckLibraryExists)
check_library_exists(m sqrt "" LIBM)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(memory src/card.c src/deck.c src/memory.c src/settings.c)

# Link the target to the GTK+ libraries
target_link_libraries(memory ${GTK3_LIBRARIES})

if(LIBM)
    target_link_libraries(memory m)
endif()
