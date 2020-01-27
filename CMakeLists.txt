cmake_minimum_required(VERSION 3.5)
project(utf VERSION 0.0.1 LANGUAGES CXX)

option(UTF_BUILD_TESTS "Build UTF test suite" ON)
option(UTF_SINGLE_INCLUDE "Use single include variant" OFF)

set(UTF_TARGET_NAME ${PROJECT_NAME})
set(UTF_CMAKE_CONFIG_DIR          "${CMAKE_CURRENT_BINARY_DIR}")
set(UTF_TARGET_EXPORT_NAME "${PROJECT_NAME}-targets")
set(UTF_TARGETS_FILE "${UTF_CMAKE_CONFIG_DIR}/${PROJECT_NAME}-targets.cmake")
set(UTF_CONFIG_FILE "${UTF_CMAKE_CONFIG_DIR}/${PROJECT_NAME}-config.cmake")
set(UTF_VERSION_FILE "${UTF_CMAKE_CONFIG_DIR}/${PROJECT_NAME}-version.cmake")
set(UTF_INCLUDE_INSTALL_DIR "include")

if (UTF_SINGLE_INCLUDE)
    message(STATUS "Using single UTF include variant")
    set(UTF_INCLUDE_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/single_include/)
else()
    message(STATUS "Using multiple UTF includes variant")
    set(UTF_INCLUDE_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
endif()

set(UTF_CONFIG_INSTALL_DIR "lib/cmake/${PROJECT_NAME}")

add_library(${UTF_TARGET_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${UTF_TARGET_NAME} ALIAS ${UTF_TARGET_NAME})

target_include_directories(
    ${UTF_TARGET_NAME} 
    INTERFACE 
    $<BUILD_INTERFACE:${UTF_INCLUDE_BUILD_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(
    ${UTF_TARGET_NAME} 
    INTERFACE
    cxx_variadic_templates
    cxx_variable_templates
)
        

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${UTF_VERSION_FILE} COMPATIBILITY AnyNewerVersion
)

configure_package_config_file( 
    ${CMAKE_CURRENT_SOURCE_DIR}/script/${PROJECT_NAME}-config.cmake.in
    ${UTF_CONFIG_FILE}
    INSTALL_DESTINATION 
    ${UTF_CONFIG_INSTALL_DIR}
)

include(CTest)
if (BUILD_TESTING AND UTF_BUILD_TESTS)
    enable_testing()
    message(STATUS "Building UTF tests")
    add_subdirectory(test)
endif()

install(
    DIRECTORY ${UTF_INCLUDE_BUILD_DIR}
    DESTINATION ${UTF_INCLUDE_INSTALL_DIR}
)
install(
    FILES ${UTF_CONFIG_FILE} ${UTF_VERSION_FILE}
    DESTINATION ${UTF_CONFIG_INSTALL_DIR}
)        
  
export(
    TARGETS ${UTF_TARGET_NAME}
    NAMESPACE ${PROJECT_NAME}::
    FILE ${UTF_TARGETS_FILE}
)

install(
    TARGETS ${UTF_TARGET_NAME}
    EXPORT ${UTF_TARGET_EXPORT_NAME}
    INCLUDES DESTINATION ${UTF_INCLUDE_INSTALL_DIR}
)

install(
   EXPORT ${UTF_TARGET_EXPORT_NAME}
   NAMESPACE ${PROJECT_NAME}::
   DESTINATION ${UTF_CONFIG_INSTALL_DIR}
)
