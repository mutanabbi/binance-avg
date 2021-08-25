set(NAME ${CMAKE_PROJECT_NAME})
add_executable(${NAME}
    book.cpp
    collector.cpp
    consumer.cpp
    main.cpp
    model/ask.cpp
    model/bid.cpp
    model/depth_update.cpp
    model/order.cpp
    utils/interval_timer.cpp
)
target_link_libraries(${NAME}
    ${BOOST_ASIO_LIBRARY}
    ${OPENSSL_LIBRARIES}
    Threads::Threads
)

add_executable(
    unit-tests
    tests/average_monoid_test.cpp
    tests/book_test.cpp
    book.cpp
    # TODO: Static lib
    model/ask.cpp
    model/bid.cpp
    model/order.cpp
)
target_link_libraries(
    unit-tests
    PRIVATE
        GTest::GTest
        GTest::Main
)

gtest_discover_tests(unit-tests)
