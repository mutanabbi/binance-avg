set(NAME ${CMAKE_PROJECT_NAME})
add_executable(${NAME}
    collector.cpp
    consumer.cpp
    model/depth_update.cpp
    main.cpp
)
target_link_libraries(${NAME}
    ${BOOST_ASIO_LIBRARY}
    ${OPENSSL_LIBRARIES}
    Threads::Threads
)

add_executable(
    unit-tests
    tests/average_monoid_test.cpp
)
target_link_libraries(
    unit-tests
    PRIVATE
        GTest::GTest
        GTest::Main
)
gtest_discover_tests(unit-tests)
