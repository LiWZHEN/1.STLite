include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/data)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/corner_data)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/self_easy_test)


add_executable(map_easy ${CMAKE_CURRENT_SOURCE_DIR}/self_easy_test/main.cpp)

add_executable(map_one ${CMAKE_CURRENT_SOURCE_DIR}/data/one/code.cpp)
add_executable(map_two ${CMAKE_CURRENT_SOURCE_DIR}/data/two/code.cpp)
add_executable(map_three ${CMAKE_CURRENT_SOURCE_DIR}/data/three/code.cpp)
add_executable(map_four ${CMAKE_CURRENT_SOURCE_DIR}/data/four/code.cpp)
add_executable(map_five ${CMAKE_CURRENT_SOURCE_DIR}/data/five/code.cpp)

add_executable(map_one_memcheck ${CMAKE_CURRENT_SOURCE_DIR}/data/one.memcheck/code.cpp)
add_executable(map_two_memcheck ${CMAKE_CURRENT_SOURCE_DIR}/data/two.memcheck/code.cpp)
add_executable(map_three_memcheck ${CMAKE_CURRENT_SOURCE_DIR}/data/three.memcheck/code.cpp)
add_executable(map_four_memcheck ${CMAKE_CURRENT_SOURCE_DIR}/data/four.memcheck/code.cpp)
add_executable(map_five_memcheck ${CMAKE_CURRENT_SOURCE_DIR}/data/five.memcheck/code.cpp)

add_executable(map_corner_one ${CMAKE_CURRENT_SOURCE_DIR}/corner_data/1.cpp)
add_executable(map_corner_two ${CMAKE_CURRENT_SOURCE_DIR}/corner_data/2.cpp)
add_executable(map_corner_three ${CMAKE_CURRENT_SOURCE_DIR}/corner_data/3.cpp)


add_test(NAME map_easy COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_easy >/tmp/easy_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/self_easy_test/answer.txt /tmp/easy_out.txt>/tmp/easy_diff.txt")

add_test(NAME map_one COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_one >/tmp/one_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/data/one/answer.txt /tmp/one_out.txt>/tmp/one_diff.txt")
add_test(NAME map_two COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_two >/tmp/two_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/data/two/answer.txt /tmp/two_out.txt>/tmp/two_diff.txt")
add_test(NAME map_three COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_three >/tmp/three_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/data/three/answer.txt /tmp/three_out.txt>/tmp/three_diff.txt")
add_test(NAME map_four COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_four >/tmp/four_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/data/four/answer.txt /tmp/four_out.txt>/tmp/four_diff.txt")
add_test(NAME map_five COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_five >/tmp/five_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/data/five/answer.txt /tmp/five_out.txt>/tmp/five_diff.txt")

add_test(NAME map_one_memcheck COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_one_memcheck >/tmp/one_memcheck_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/data/one_memcheck/answer.txt /tmp/one_memcheck_out.txt>/tmp/one_memcheck_diff.txt")
add_test(NAME map_two_memcheck COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_two_memcheck >/tmp/two_memcheck_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/data/two_memcheck/answer.txt /tmp/two_memcheck_out.txt>/tmp/two_memcheck_diff.txt")
add_test(NAME map_three_memcheck COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_three_memcheck >/tmp/three_memcheck_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/data/three_memcheck/answer.txt /tmp/three_memcheck_out.txt>/tmp/three_memcheck_diff.txt")
add_test(NAME map_four_memcheck COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_four_memcheck >/tmp/four_memcheck_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/data/four_memcheck/answer.txt /tmp/four_memcheck_out.txt>/tmp/four_memcheck_diff.txt")
add_test(NAME map_five_memcheck COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_five_memcheck >/tmp/five_memcheck_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/data/five_memcheck/answer.txt /tmp/five_memcheck_out.txt>/tmp/five_memcheck_diff.txt")

add_test(NAME map_corner_one COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_corner_one >/tmp/one_corner_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/corner_data/1.txt /tmp/one_corner_out.txt>/tmp/one_corner_diff.txt")
add_test(NAME map_corner_two COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_corner_two >/tmp/two_corner_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/corner_data/2.txt /tmp/two_corner_out.txt>/tmp/two_corner_diff.txt")
add_test(NAME map_corner_three COMMAND sh -c "${CMAKE_CURRENT_BINARY_DIR}/map_corner_three >/tmp/three_corner_out.txt\
        && diff -u ${CMAKE_CURRENT_SOURCE_DIR}/corner_data/3.txt /tmp/three_corner_out.txt>/tmp/three_corner_diff.txt")