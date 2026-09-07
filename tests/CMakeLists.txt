cmake_minimum_required(VERSION 3.11)
project(nfiq2_data_tests LANGUAGES CXX)

enable_testing()
add_executable(nfiq2_data_tests
    data.cpp
    ../NFIQ2/NFIQ2Algorithm/src/nfiq2/nfiq2_data.cpp
    ../NFIQ2/NFIQ2Algorithm/src/nfiq2/nfiq2_exception.cpp)
target_include_directories(nfiq2_data_tests PRIVATE
    ../NFIQ2/NFIQ2Algorithm/include)
set_target_properties(nfiq2_data_tests PROPERTIES
    CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
add_test(NAME data COMMAND nfiq2_data_tests)
