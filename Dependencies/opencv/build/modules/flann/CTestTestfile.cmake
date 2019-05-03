# CMake generated Testfile for 
# Source directory: D:/dev/plise_work_dammit/Dependencies/opencv/sources/modules/flann
# Build directory: D:/dev/plise_work_dammit/Dependencies/opencv/build/modules/flann
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(opencv_test_flann "D:/dev/plise_work_dammit/Dependencies/opencv/build/bin/Debug/opencv_test_flannd.exe" "--gtest_output=xml:opencv_test_flann.xml")
  set_tests_properties(opencv_test_flann PROPERTIES  LABELS "Main;opencv_flann;Accuracy" WORKING_DIRECTORY "D:/dev/plise_work_dammit/Dependencies/opencv/build/test-reports/accuracy" _BACKTRACE_TRIPLES "D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVUtils.cmake;1547;add_test;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1282;ocv_add_test_from_target;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1069;ocv_add_accuracy_tests;D:/dev/plise_work_dammit/Dependencies/opencv/sources/modules/flann/CMakeLists.txt;2;ocv_define_module;D:/dev/plise_work_dammit/Dependencies/opencv/sources/modules/flann/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(opencv_test_flann "D:/dev/plise_work_dammit/Dependencies/opencv/build/bin/Release/opencv_test_flann.exe" "--gtest_output=xml:opencv_test_flann.xml")
  set_tests_properties(opencv_test_flann PROPERTIES  LABELS "Main;opencv_flann;Accuracy" WORKING_DIRECTORY "D:/dev/plise_work_dammit/Dependencies/opencv/build/test-reports/accuracy" _BACKTRACE_TRIPLES "D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVUtils.cmake;1547;add_test;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1282;ocv_add_test_from_target;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1069;ocv_add_accuracy_tests;D:/dev/plise_work_dammit/Dependencies/opencv/sources/modules/flann/CMakeLists.txt;2;ocv_define_module;D:/dev/plise_work_dammit/Dependencies/opencv/sources/modules/flann/CMakeLists.txt;0;")
else()
  add_test(opencv_test_flann NOT_AVAILABLE)
endif()
