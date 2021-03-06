cmake_minimum_required(VERSION 2.6)

set(CTEST_PROJECT_NAME "DistortionCompensation")

PROJECT(DistortionCompensation)

ENABLE_TESTING()

IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "-Wall -Werror -pedantic -O2")
ENDIF()

ADD_EXECUTABLE(test_smoother test_smoother.c smoother.c)
ADD_EXECUTABLE(test_smoother_integral test_smoother_integral.c smoother.c)

ADD_EXECUTABLE(test_filter test_filter.c filter.c)

ADD_EXECUTABLE(test_undistorter test_undistorter.c filter.c smoother.c undistorter.c)
ADD_EXECUTABLE(test_undistorter_thd test_undistorter_thd.c filter.c smoother.c undistorter.c)

ADD_TEST(SmootherLinearMSE test_smoother)
SET_TESTS_PROPERTIES(SmootherLinearMSE
    PROPERTIES PASS_REGULAR_EXPRESSION "PASS")
    
ADD_TEST(SmootherIntegratedLinearMSE test_smoother_integral)
SET_TESTS_PROPERTIES(SmootherIntegratedLinearMSE
    PROPERTIES PASS_REGULAR_EXPRESSION "PASS")
    
ADD_TEST(FilterFrequencyResponse test_filter)
SET_TESTS_PROPERTIES(FilterFrequencyResponse
    PROPERTIES PASS_REGULAR_EXPRESSION "PASS")
    
ADD_TEST(UndistorterTHD test_undistorter_thd)
SET_TESTS_PROPERTIES(UndistorterTHD
    PROPERTIES PASS_REGULAR_EXPRESSION "PASS")