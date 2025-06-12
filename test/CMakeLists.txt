# Find all .oberon test files
file(GLOB TEST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.oberon")

message(STATUS "Looking for .oberon files in: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Files found: ${TEST_FILES}")

foreach(test_file ${TEST_FILES})
    get_filename_component(test_name ${test_file} NAME_WE)
    set(expected_output ${CMAKE_CURRENT_SOURCE_DIR}/${test_name}.expected)

    if(EXISTS ${expected_output})
        add_test(
            NAME ${test_name}
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run_test.sh
                ${CMAKE_BINARY_DIR}/bin/oberon07_llvm
                ${CMAKE_CURRENT_SOURCE_DIR}/${test_file}
                ${test_name}
                ${expected_output}
        )
        set_property(TEST ${test_name} PROPERTY WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    else()
        message(WARNING "Missing expected output for ${test_name}")
    endif()
endforeach()
