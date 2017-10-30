set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/test_bin)

add_subdirectory(consensus)

# TODO: now it does not work.
# @warchant: I disabled this test, because crypto and
# genesis block generator should be changed
#add_subdirectory(pipeline)

addtest(client_test client_test.cpp)
target_link_libraries(client_test
    pb_model_converters
    json_model_converters
    client
    processors
    server_runner
    )
target_include_directories(client_test PUBLIC
    ${PROJECT_SOURCE_DIR}/iroha-cli
    )

