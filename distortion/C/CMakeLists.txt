cmake_minimum_required(VERSION 2.6)

set(CTEST_PROJECT_NAME "DistortionCompensation")

PROJECT(DistortionCompensation)

ENABLE_TESTING()

ADD_EXECUTABLE(test_smoother test_smoother.c smoother.c)
ADD_EXECUTABLE(test_smoother_integral test_smoother_integral.c smoother.c)

ADD_TEST(SmootherLinearMSE test_smoother)
SET_TESTS_PROPERTIES(SmootherLinearMSE
    PROPERTIES PASS_REGULAR_EXPRESSION "PASS")
    
ADD_TEST(SmootherIntegratedLinearMSE test_smoother_integral)
SET_TESTS_PROPERTIES(SmootherIntegratedLinearMSE
    PROPERTIES PASS_REGULAR_EXPRESSION "PASS")