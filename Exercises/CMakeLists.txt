cmake_minimum_required(VERSION 3.10)
project(TxRxApplication C)

add_executable(TxRxApplication TxRxApplication.c)

target_link_libraries(TxRxApplication PRIVATE AES_Library)
