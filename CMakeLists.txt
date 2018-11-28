cmake_minimum_required(VERSION 3.3)
project(huffman CXX)
include(.ipd/cmake/CMakeLists.txt)

set(COMMON src/bit_io.cpp src/common.h)
set(SEVEN_COMMON ${COMMON} src/7coder.cpp)

# List files for building huff, puff, and huffman_test here, and they will be
# included in all three:
set(HUFF_COMMON
        ${COMMON})

add_program(encode src/encode.cpp ${SEVEN_COMMON})
add_program(decode src/decode.cpp ${SEVEN_COMMON})
add_program(huff src/huff.cpp ${HUFF_COMMON})
add_program(puff src/puff.cpp ${HUFF_COMMON})

add_test_program(bitio_test src/bit_io.cpp test/bitio_test.cpp)
add_test_program(7coder_test test/7coder_test.cpp ${SEVEN_COMMON})
add_test_program(huffman_test test/huffman_test.cpp ${HUFF_COMMON})
