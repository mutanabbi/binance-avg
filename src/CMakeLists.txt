set(NAME ${CMAKE_PROJECT_NAME})
add_executable(${NAME}
    average_monoid.cpp
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
