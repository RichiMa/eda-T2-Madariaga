cmake_minimum_required(VERSION 3.10)

project(poscodes VERSION 1.0 LANGUAGES CXX)

# Usa C++11 (suficiente para chrono, stoull, move, etc.)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default a Release si no se setea desde afuera
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Ejecutable y fuentes
add_executable(poscodes
  src/main.cpp
  src/utils.cpp
  src/poscode.cpp
)

# Ruta a headers
target_include_directories(poscodes
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Flags útiles (opcional pero recomendado)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(poscodes PRIVATE -O3 -Wall -Wextra -Wno-sign-compare)
elseif(MSVC)
  target_compile_options(poscodes PRIVATE /O2 /W3)
endif()
