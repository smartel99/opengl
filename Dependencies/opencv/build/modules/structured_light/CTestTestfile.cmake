# CMake generated Testfile for 
# Source directory: C:/Users/steph/Desktop/opencv_contrib-master/modules/structured_light
# Build directory: D:/dev/plise_work_dammit/Dependencies/opencv/build/modules/structured_light
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(opencv_test_structured_light "D:/dev/plise_work_dammit/Dependencies/opencv/build/bin/Debug/opencv_test_structured_lightd.exe" "--gtest_output=xml:opencv_test_structured_light.xml")
  set_tests_properties(opencv_test_structured_light PROPERTIES  LABELS "Extra;opencv_structured_light;Accuracy" WORKING_DIRECTORY "D:/dev/plise_work_dammit/Dependencies/opencv/build/test-reports/accuracy" _BACKTRACE_TRIPLES "D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVUtils.cmake;1547;add_test;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1282;ocv_add_test_from_target;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1069;ocv_add_accuracy_tests;C:/Users/steph/Desktop/opencv_contrib-master/modules/structured_light/CMakeLists.txt;2;ocv_define_module;C:/Users/steph/Desktop/opencv_contrib-master/modules/structured_light/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(opencv_test_structured_light "D:/dev/plise_work_dammit/Dependencies/opencv/build/bin/Release/opencv_test_structured_light.exe" "--gtest_output=xml:opencv_test_structured_light.xml")
  set_tests_properties(opencv_test_structured_light PROPERTIES  LABELS "Extra;opencv_structured_light;Accuracy" WORKING_DIRECTORY "D:/dev/plise_work_dammit/Dependencies/opencv/build/test-reports/accuracy" _BACKTRACE_TRIPLES "D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVUtils.cmake;1547;add_test;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1282;ocv_add_test_from_target;D:/dev/plise_work_dammit/Dependencies/opencv/sources/cmake/OpenCVModule.cmake;1069;ocv_add_accuracy_tests;C:/Users/steph/Desktop/opencv_contrib-master/modules/structured_light/CMakeLists.txt;2;ocv_define_module;C:/Users/steph/Desktop/opencv_contrib-master/modules/structured_light/CMakeLists.txt;0;")
else()
  add_test(opencv_test_structured_light NOT_AVAILABLE)
endif()