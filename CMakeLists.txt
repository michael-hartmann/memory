project(MemoryGtk3 C)
cmake_minimum_required(VERSION 3.12)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(memory src/card.c src/card.h src/deck.c src/deck.h src/memory.c src/memory.h src/settings.c src/settings.h)

# Link the target to the GTK+ libraries
target_link_libraries(memory ${GTK3_LIBRARIES})
