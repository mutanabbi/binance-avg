set(NAME ${CMAKE_PROJECT_NAME})
add_executable(${NAME}
    main.cpp
)
target_link_libraries(${NAME}
    ${BOOST_ASIO_LIBRARY}
    ${OPENSSL_LIBRARIES}
    Threads::Threads
)
