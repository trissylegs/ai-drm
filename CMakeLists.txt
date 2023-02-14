cmake_minimum_required(VERSION 3.0.0)
project(kms_image VERSION 0.1.0)
# Set version
set(KMS_IMAGE_VERSION_MAJOR 0)
set(KMS_IMAGE_VERSION_MINOR 1)
set(KMS_IMAGE_VERSION_PATCH 0)
set(KMS_IMAGE_VERSION "${KMS_IMAGE_VERSION_MAJOR}.${KMS_IMAGE_VERSION_MINOR}.${KMS_IMAGE_VERSION_PATCH}")

include(CTest)
enable_testing()

# Generate version.h
configure_file(
  "${PROJECT_SOURCE_DIR}/version.h.in"
  "${PROJECT_BINARY_DIR}/version.h"
)
include_directories("${PROJECT_BINARY_DIR}")


function(print_dirs var name)
    foreach (path ${var})
        message(STATUS "${name}: " ${path})
    endforeach ()
endfunction()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Boost
# * filesystem for loading images
# * program_options for parsing command line arguments
find_package(Boost REQUIRED COMPONENTS filesystem program_options)

print_dirs("${Boost_INCLUDE_DIRS}" "Boost_INCLUDE_DIRS")
print_dirs("${Boost_LIBRARIES}" "Boost_LIBRARIES")

# Boost includes
include_directories(${Boost_INCLUDE_DIRS})

# Executuable
add_executable(kms_image main.cpp)
target_link_libraries(kms_image ${Boost_LIBRARIES})


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
