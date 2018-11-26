
add_library(test_main STATIC
    "src/test.cpp"
)
add_library(test_main::test_main ALIAS test_main)
target_include_directories(
    test_main
    PUBLIC
    "thirdparty/catch"
)
target_compile_features(
    test_main
    PUBLIC
    cxx_variadic_templates
    cxx_variable_templates
)
file(GLOB test_files "src/test_*.cpp")
foreach(test_file ${test_files})
    get_filename_component(test_target ${test_file} NAME_WE)
    
    add_executable(${test_target} ${test_file})
    
    target_link_libraries(${test_target} test_main::test_main utf::utf)

    add_test(
        NAME ${test_target}
        COMMAND ${test_target} --use-colour yes
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )

endforeach()
